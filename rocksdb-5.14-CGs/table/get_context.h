//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once
#include <string>
#include "db/merge_context.h"
#include "db/range_del_aggregator.h"
#include "db/read_callback.h"
#include "rocksdb/env.h"
#include "rocksdb/statistics.h"
#include "rocksdb/types.h"
#include "table/block.h"

namespace rocksdb {
class MergeContext;
class PinnedIteratorsManager;

class GetContext {
 public:
  enum GetState {
    kNotFound,
    kFound,
    kDeleted,
    kCorrupt,
    kMerge,  // saver contains the current merge result (the operands)
    kBlobIndex,
  };
  uint64_t tickers_value[Tickers::TICKER_ENUM_MAX] = {0};

  GetContext(const Comparator* ucmp, const MergeOperator* merge_operator,
             Logger* logger, Statistics* statistics, GetState init_state,
             const Slice& user_key, PinnableSlice* value, bool* value_found,
             MergeContext* merge_context, RangeDelAggregator* range_del_agg,
             Env* env, int required_columns, uint32_t column_num, char delim, 
             const std::vector<std::vector<std::tuple<uint32_t, uint32_t>>>& cg_range_matrix,
             std::set<uint32_t>* columns_to_be_fetched,
             SequenceNumber* seq = nullptr, PinnedIteratorsManager* _pinned_iters_mgr = nullptr,
             ReadCallback* callback = nullptr, bool* is_blob_index = nullptr);

  void MarkKeyMayExist();

  // Records this key, value, and any meta-data (such as sequence number and
  // state) into this GetContext.
  //
  // If the parsed_key matches the user key that we are looking for, sets
  // mathced to true.
  //
  // Returns True if more keys need to be read (due to merges) or
  //         False if the complete value has been found.
  bool SaveValue(const ParsedInternalKey& parsed_key, const Slice& value,
                 bool* matched, int col_num, int level, Cleanable* value_pinner = nullptr);

  // Simplified version of the previous function. Should only be used when we
  // know that the operation is a Put.
  void SaveValue(const Slice& value, SequenceNumber seq);

  GetState State() const { return state_; }

  RangeDelAggregator* range_del_agg() { return range_del_agg_; }

  PinnedIteratorsManager* pinned_iters_mgr() { return pinned_iters_mgr_; }

  // If a non-null string is passed, all the SaveValue calls will be
  // logged into the string. The operations can then be replayed on
  // another GetContext with replayGetContextLog.
  void SetReplayLog(std::string* replay_log) { replay_log_ = replay_log; }

  // Do we need to fetch the SequenceNumber for this key?
  bool NeedToReadSequence() const { return (seq_ != nullptr); }

  bool sample() const { return sample_; }

  bool CheckCallback(SequenceNumber seq) {
    if (callback_) {
      return callback_->IsCommitted(seq);
    }
    return true;
  }

  void RecordCounters(Tickers ticker, size_t val);

  // HEMANT: convert vector of column values to delim separated string
  void saveVecToString(std::string* result, std::vector<std::string>& vec);
  // HEMANT: check if we have found a value for a given col
  bool haveValForCol(int col) {
    if(saved_vals_.at(col).empty()) {
      return false;
    } else {
      return true;
    }
  }

 private:
  const Comparator* ucmp_;
  const MergeOperator* merge_operator_;
  // the merge operations encountered;
  Logger* logger_;
  Statistics* statistics_;

  GetState state_;
  Slice user_key_;
  PinnableSlice* pinnable_val_;
  bool* value_found_;  // Is value set correctly? Used by KeyMayExist
  MergeContext* merge_context_;
  RangeDelAggregator* range_del_agg_;
  Env* env_;
  int required_columns_; // HEMANT: to keep track of number of column values fetched
  uint32_t column_num_; // HEMANT: total number of columns
  char delim_;  // HEMANT
  std::vector<std::vector<std::tuple<uint32_t, uint32_t>>> cg_range_matrix_; // HEMANT: (UPDATE for CGs)
  std::set<uint32_t>* columns_to_be_fetched_;
  // If a key is found, seq_ will be set to the SequenceNumber of most recent
  // write to the key or kMaxSequenceNumber if unknown
  SequenceNumber* seq_;
  std::string* replay_log_;
  // Used to temporarily pin blocks when state_ == GetContext::kMerge
  PinnedIteratorsManager* pinned_iters_mgr_;
  ReadCallback* callback_;
  bool sample_;
  bool* is_blob_index_;
  int saved_columns_; // HEMANT: keeps track of  how many column values saved
  std::vector<std::string> saved_vals_; // HEMANT: save column values for respective columns
  std::vector<std::string> merge_vals_; // HEMANT: temporarily save column values for merge operands
};

void replayGetContextLog(const Slice& replay_log, const Slice& user_key,
                         GetContext* get_context,
                         Cleanable* value_pinner = nullptr);

}  // namespace rocksdb
