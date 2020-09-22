//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "table/get_context.h"
#include "db/merge_helper.h"
#include "db/pinned_iterators_manager.h"
#include "db/read_callback.h"
#include "monitoring/file_read_sample.h"
#include "monitoring/perf_context_imp.h"
#include "monitoring/statistics.h"
#include "rocksdb/env.h"
#include "rocksdb/merge_operator.h"
#include "rocksdb/statistics.h"

namespace rocksdb {

namespace {

void appendToReplayLog(std::string* replay_log, ValueType type, Slice value) {
#ifndef ROCKSDB_LITE
  if (replay_log) {
    if (replay_log->empty()) {
      // Optimization: in the common case of only one operation in the
      // log, we allocate the exact amount of space needed.
      replay_log->reserve(1 + VarintLength(value.size()) + value.size());
    }
    replay_log->push_back(type);
    PutLengthPrefixedSlice(replay_log, value);
  }
#else
  (void)replay_log;
  (void)type;
  (void)value;
#endif  // ROCKSDB_LITE
}

}  // namespace

GetContext::GetContext(const Comparator* ucmp,
                       const MergeOperator* merge_operator, Logger* logger,
                       Statistics* statistics, GetState init_state,
                       const Slice& user_key, PinnableSlice* pinnable_val,
                       bool* value_found, MergeContext* merge_context,
                       RangeDelAggregator* _range_del_agg, Env* env,
                       int required_columns, uint32_t column_num, 
                       char delim, const std::vector<std::vector<std::tuple<uint32_t, uint32_t>>>& cg_range_matrix, 
                       std::set<uint32_t>* columns_to_be_fetched, SequenceNumber* seq, PinnedIteratorsManager* _pinned_iters_mgr,
                       ReadCallback* callback, bool* is_blob_index)
    : ucmp_(ucmp),
      merge_operator_(merge_operator),
      logger_(logger),
      statistics_(statistics),
      state_(init_state),
      user_key_(user_key),
      pinnable_val_(pinnable_val),
      value_found_(value_found),
      merge_context_(merge_context),
      range_del_agg_(_range_del_agg),
      env_(env),
      required_columns_(required_columns),
      column_num_(column_num),
      delim_(delim),
      cg_range_matrix_(cg_range_matrix),
      columns_to_be_fetched_(columns_to_be_fetched),
      seq_(seq),
      replay_log_(nullptr),
      pinned_iters_mgr_(_pinned_iters_mgr),
      callback_(callback),
      is_blob_index_(is_blob_index) {
  if (seq_) {
    *seq_ = kMaxSequenceNumber;
  }
  sample_ = should_sample_file_read();
  saved_columns_ = 0;
  saved_vals_.resize(column_num_);
  merge_vals_.resize(column_num_);
}

// Called from TableCache::Get and Table::Get when file/block in which
// key may exist are not there in TableCache/BlockCache respectively. In this
// case we can't guarantee that key does not exist and are not permitted to do
// IO to be certain.Set the status=kFound and value_found=false to let the
// caller know that key may exist but is not there in memory
void GetContext::MarkKeyMayExist() {
  state_ = kFound;
  if (value_found_ != nullptr) {
    *value_found_ = false;
  }
}

void GetContext::SaveValue(const Slice& value, SequenceNumber /*seq*/) {
  assert(state_ == kNotFound);
  appendToReplayLog(replay_log_, kTypeValue, value);

  state_ = kFound;
  if (LIKELY(pinnable_val_ != nullptr)) {
    pinnable_val_->PinSelf(value);
  }
}

void GetContext::RecordCounters(Tickers ticker, size_t val) {
  if (ticker == Tickers::TICKER_ENUM_MAX) {
    return;
  }
  tickers_value[ticker] += static_cast<uint64_t>(val);
}

bool GetContext::SaveValue(const ParsedInternalKey& parsed_key,
                           const Slice& value, bool* matched,
                           int col_num, int level, Cleanable* value_pinner) { // HEMANT
  assert(matched);
  assert((state_ != kMerge && parsed_key.type != kTypeMerge) ||
         merge_context_ != nullptr);
  // UPDATE for CGs: the col_num is CG_num now, don't use it as it is to assign value to saved_vals_
  // get the columns covered by this CG_num, and save value in those columns
  // But level number doesn't seem to be available here. It is need to get CG range vec from cg_matrix
  assert(col_num >= 0); // HEMANT
  if (ucmp_->Equal(parsed_key.user_key, user_key_)) {
    *matched = true;
    // If the value is not in the snapshot, skip it
    if (!CheckCallback(parsed_key.sequence)) {
      return true;  // to continue to the next seq
    }

    // UPDATE for CGs: level 0 is always row. And we need to substract 1 from col_num returned for
    // any file in level 1 onwards.
    std::tuple<uint32_t, uint32_t> cg_range = std::make_tuple(0, column_num_-1);
    bool is_complete_row = true;
    if(level > 0) {
      is_complete_row = (cg_range_matrix_.at(level).size() == 1) ? true : false;
      cg_range = cg_range_matrix_.at(level).at(col_num-1);
    }

    appendToReplayLog(replay_log_, parsed_key.type, value);

    if (seq_ != nullptr) {
      // Set the sequence number if it is uninitialized
      if (*seq_ == kMaxSequenceNumber) {
        *seq_ = parsed_key.sequence;
      }
    }

    auto type = parsed_key.type;
    // Key matches. Process it
    if ((type == kTypeValue || type == kTypeMerge || type == kTypeBlobIndex) &&
        range_del_agg_ != nullptr && range_del_agg_->ShouldDelete(parsed_key)) {
      type = kTypeRangeDeletion;
    }
    switch (type) {
      case kTypeValue:
      case kTypeBlobIndex:
        assert(state_ == kNotFound || state_ == kMerge || saved_columns_ < required_columns_);  // HEMANT
        if (type == kTypeBlobIndex && is_blob_index_ == nullptr) {
          // Blob value not supported. Stop.
          state_ = kBlobIndex;
          return false;
        }
        if (kNotFound == state_) {
          state_ = kFound;
          if(!is_complete_row /* col_num > 0 */) {
            // column values save them in a vector
            // UPDATE for CGs: we should save all the column values that are present in col_num CG
            std::vector<std::string> vals(std::get<1>(cg_range) - std::get<0>(cg_range) + 1);
            value.split(delim_, vals);
            for(uint32_t i = std::get<0>(cg_range); i <= std::get<1>(cg_range); i++) {
              if (columns_to_be_fetched_->find(i+1) != columns_to_be_fetched_->end()) {
                saved_vals_[i].assign(vals[i - std::get<0>(cg_range)]);
                saved_columns_++;
              }
            }
          } else {
            // its a row value, save to pinnable_val
            if (LIKELY(pinnable_val_ != nullptr)) {
              if (LIKELY(value_pinner != nullptr)) {
                // If the backing resources for the value are provided, pin them
                //pinnable_val_->PinSlice(value, value_pinner);
                pinnable_val_->PinSelf(value);  // HEMANT: disabled pinning with value_pinner, pinnning to self makes append easy
              } else {
                // Otherwise copy the value
                pinnable_val_->PinSelf(value);
              }
              saved_columns_++; // HEMANT
            }
          }
          
        } else if (kMerge == state_) {
          // HEMANT: if we got a column value and haven't received all the column values,
          // save the column value and continue fetching more column values
          if(!is_complete_row /* col_num > 0 */ && saved_columns_ < required_columns_) {
            std::vector<std::string> vals(std::get<1>(cg_range) - std::get<0>(cg_range) + 1);
            value.split(delim_, vals);
            for(uint32_t i = std::get<0>(cg_range); i <= std::get<1>(cg_range); i++) {
              if (columns_to_be_fetched_->find(i+1) != columns_to_be_fetched_->end()) {
                saved_vals_[i].assign(vals[i - std::get<0>(cg_range)]);
                saved_columns_++;
              }
            }
            state_ = kFound;
          } else {
            // Either we are about to merge a row style value, 
            // or we have collected all the column values
            assert(merge_operator_ != nullptr);
            state_ = kFound;
            if (LIKELY(pinnable_val_ != nullptr)) {
              if(!is_complete_row /* col_num > 0 */) {
                // column style value, construct the row and then merge
                std::string complete_val;
                saveVecToString(&complete_val, saved_vals_);
                Slice complete_val_slice(complete_val);
                PERF_COUNTER_ADD(h2saxena_merge_for_get, merge_context_->GetNumOperands());
                Status merge_status = MergeHelper::TimedFullMerge(
                    merge_operator_, user_key_, &complete_val_slice,
                    merge_context_->GetOperands(), pinnable_val_->GetSelf(),
                    logger_, statistics_, env_);
                pinnable_val_->PinSelf();
                if (!merge_status.ok()) {
                  state_ = kCorrupt;
                }
              } else {
                // its a row style value, merge the value right away
                PERF_COUNTER_ADD(h2saxena_merge_for_get, merge_context_->GetNumOperands());
                Status merge_status = MergeHelper::TimedFullMerge(
                    merge_operator_, user_key_, &value,
                    merge_context_->GetOperands(), pinnable_val_->GetSelf(),
                    logger_, statistics_, env_);
                pinnable_val_->PinSelf();
                if (!merge_status.ok()) {
                  state_ = kCorrupt;
                }
              }
            }
          }
        } else if (kFound == state_ && saved_columns_ < required_columns_) {
          // Append other column values
          //pinnable_val_->UpdateSelf(value);
          std::vector<std::string> vals(std::get<1>(cg_range) - std::get<0>(cg_range) + 1);
          value.split(delim_, vals);
          for(uint32_t i = std::get<0>(cg_range); i <= std::get<1>(cg_range); i++) {
            if (columns_to_be_fetched_->find(i+1) != columns_to_be_fetched_->end()) {
              saved_vals_[i].assign(vals[i - std::get<0>(cg_range)]);
              saved_columns_++;
            }
          }
          // If we have fetched everything save it to pinnable_val_
          if(saved_columns_ == required_columns_) {
            std::string complete_val;
            saveVecToString(&complete_val, saved_vals_);
            if(merge_context_->GetNumOperands() > 0) {
              Slice complete_val_slice(complete_val);
              PERF_COUNTER_ADD(h2saxena_merge_for_get, merge_context_->GetNumOperands());
              Status merge_status = MergeHelper::TimedFullMerge(
                  merge_operator_, user_key_, &complete_val_slice,
                  merge_context_->GetOperands(), pinnable_val_->GetSelf(),
                  logger_, statistics_, env_);
              pinnable_val_->PinSelf();
              if (!merge_status.ok()) {
                state_ = kCorrupt;
              }
            } else {
              pinnable_val_->PinSelf(complete_val);
            }
          }
        }

        if (is_blob_index_ != nullptr) {
          *is_blob_index_ = (type == kTypeBlobIndex);
        }
        return false;

      case kTypeDeletion:
      case kTypeSingleDeletion:
      case kTypeRangeDeletion:
        // TODO(noetzli): Verify correctness once merge of single-deletes
        // is supported
        assert(state_ == kNotFound || state_ == kMerge);
        if (kNotFound == state_) {
          state_ = kDeleted;
        } else if (kMerge == state_) {
          state_ = kFound;
          if (LIKELY(pinnable_val_ != nullptr)) {
            Status merge_status = MergeHelper::TimedFullMerge(
                merge_operator_, user_key_, nullptr,
                merge_context_->GetOperands(), pinnable_val_->GetSelf(),
                logger_, statistics_, env_);
            pinnable_val_->PinSelf();
            if (!merge_status.ok()) {
              state_ = kCorrupt;
            }
          }
        }
        return false;

      case kTypeMerge:
        assert(state_ == kNotFound || state_ == kMerge);
        state_ = kMerge;
        // value_pinner is not set from plain_table_reader.cc for example.
        // HEMANT: if value is coming from a row level then we are guaranteed 
        // to have all the column values in value, so add it to Operands as it is
        // else complete the columns 
        if(is_complete_row/* col_num == 0 */) {
          if (pinned_iters_mgr() && pinned_iters_mgr()->PinningEnabled() &&
              value_pinner != nullptr) {
            value_pinner->DelegateCleanupsTo(pinned_iters_mgr());
            merge_context_->PushOperand(value, true /*value_pinned*/);
          } else {
            merge_context_->PushOperand(value, false);
          }
        } else {
          // this part constructs rows one column at a time, so will be very inefficient, 
          // but we hope that this will happen fewer times, the Get() operations should find most of the 
          // updates and values in row format. Get() op fetching data from column levels should be rare

          // contruct incomplete row
          merge_vals_.clear();
          // UPDATE for CGs: 
          std::vector<std::string> vals(std::get<1>(cg_range) - std::get<0>(cg_range) + 1);
          value.split(delim_, vals);
          for(uint32_t i = std::get<0>(cg_range); i <= std::get<1>(cg_range); i++) {
            if (columns_to_be_fetched_->find(i+1) != columns_to_be_fetched_->end()) {
              merge_vals_[i].assign(vals[i - std::get<0>(cg_range)]);
              saved_columns_++;
            }
          }

          //merge_vals_[col_num-1].assign(value.data(), value.size());
          std::string incomplete_merge_val;
          saveVecToString(&incomplete_merge_val, merge_vals_);
          // Now push operands, we want 
          merge_context_->PushOperand(incomplete_merge_val, false);
        }
        if (merge_operator_ != nullptr &&
            merge_operator_->ShouldMerge(merge_context_->GetOperands())) {
          state_ = kFound;
          if (LIKELY(pinnable_val_ != nullptr)) {
            PERF_COUNTER_ADD(h2saxena_merge_for_get, merge_context_->GetNumOperands());
            Status merge_status = MergeHelper::TimedFullMerge(
                merge_operator_, user_key_, nullptr,
                merge_context_->GetOperands(), pinnable_val_->GetSelf(),
                logger_, statistics_, env_);
            pinnable_val_->PinSelf();
            if (!merge_status.ok()) {
              state_ = kCorrupt;
            }
          }
          return false;
        }
        return true;

      default:
        assert(false);
        break;
    }
  }

  // state_ could be Corrupt, merge or notfound
  return false;
}

void GetContext::saveVecToString(std::string* result, std::vector<std::string>& vec) {
  result->clear();
  for(uint32_t i = 0; i < column_num_; i++) {
    result->append(vec[i]);
    result->append(1, delim_);
  }
}

void replayGetContextLog(const Slice& replay_log, const Slice& user_key,
                         GetContext* get_context, Cleanable* value_pinner) {
#ifndef ROCKSDB_LITE
  Slice s = replay_log;
  while (s.size()) {
    auto type = static_cast<ValueType>(*s.data());
    s.remove_prefix(1);
    Slice value;
    bool ret = GetLengthPrefixedSlice(&s, &value);
    assert(ret);
    (void)ret;

    bool dont_care __attribute__((__unused__));
    // Since SequenceNumber is not stored and unknown, we will use
    // kMaxSequenceNumber.
    get_context->SaveValue(
        ParsedInternalKey(user_key, kMaxSequenceNumber, type), value,
        &dont_care, -1, -1, value_pinner);
  }
#else   // ROCKSDB_LITE
  (void)replay_log;
  (void)user_key;
  (void)get_context;
  (void)value_pinner;
  assert(false);
#endif  // ROCKSDB_LITE
}

}  // namespace rocksdb
