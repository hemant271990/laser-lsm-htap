//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#pragma once

#include <stdint.h>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "options/cf_options.h"
#include "rocksdb/options.h"
#include "rocksdb/persistent_cache.h"
#include "rocksdb/statistics.h"
#include "rocksdb/status.h"
#include "rocksdb/table.h"
#include "table/block.h"
#include "table/filter_block.h"
#include "table/format.h"
#include "table/persistent_cache_helper.h"
#include "table/table_properties_internal.h"
#include "table/table_reader.h"
#include "table/two_level_iterator.h"
#include "util/string_util.h"
#include "util/coding.h"
#include "util/file_reader_writer.h"

namespace rocksdb {

class BlockHandle;
class Cache;
class FilterBlockReader;
class BlockBasedFilterBlockReader;
class FullFilterBlockReader;
class Footer;
class InternalKeyComparator;
class Iterator;
class RandomAccessFile;
class TableCache;
class TableReader;
class WritableFile;
struct BlockBasedTableOptions;
struct EnvOptions;
struct ReadOptions;
class GetContext;
class InternalIterator;
class ColumnIterator; // HEMANT

using std::unique_ptr;

typedef std::vector<std::pair<std::string, std::string>> KVPairBlock;

// A Table is a sorted map from strings to strings.  Tables are
// immutable and persistent.  A Table may be safely accessed from
// multiple threads without external synchronization.
class BlockBasedTable : public TableReader {
 public:
  static const std::string kFilterBlockPrefix;
  static const std::string kFullFilterBlockPrefix;
  static const std::string kPartitionedFilterBlockPrefix;
  // The longest prefix of the cache key used to identify blocks.
  // For Posix files the unique ID is three varints.
  static const size_t kMaxCacheKeyPrefixSize = kMaxVarint64Length * 3 + 1;

  // Attempt to open the table that is stored in bytes [0..file_size)
  // of "file", and read the metadata entries necessary to allow
  // retrieving data from the table.
  //
  // If successful, returns ok and sets "*table_reader" to the newly opened
  // table.  The client should delete "*table_reader" when no longer needed.
  // If there was an error while initializing the table, sets "*table_reader"
  // to nullptr and returns a non-ok status.
  //
  // @param file must remain live while this Table is in use.
  // @param prefetch_index_and_filter_in_cache can be used to disable
  // prefetching of
  //    index and filter blocks into block cache at startup
  // @param skip_filters Disables loading/accessing the filter block. Overrides
  //    prefetch_index_and_filter_in_cache, so filter will be skipped if both
  //    are set.
  static Status Open(const ImmutableCFOptions& ioptions,
                     const EnvOptions& env_options,
                     const BlockBasedTableOptions& table_options,
                     const InternalKeyComparator& internal_key_comparator,
                     unique_ptr<RandomAccessFileReader>&& file,
                     uint64_t file_size, unique_ptr<TableReader>* table_reader,
                     const SliceTransform* prefix_extractor = nullptr,
                     bool prefetch_index_and_filter_in_cache = true,
                     bool skip_filters = false, int level = -1,
                     bool is_column_oriented = false, // HEMANT
                     const std::vector<uint32_t>& cols = std::vector<uint32_t>()); // HEMANT

  bool PrefixMayMatch(const Slice& internal_key,
                      const SliceTransform* prefix_extractor = nullptr);

  // Returns a new iterator over the table contents.
  // The result of NewIterator() is initially invalid (caller must
  // call one of the Seek methods on the iterator before using it).
  // @param skip_filters Disables loading/accessing the filter block
  InternalIterator* NewIterator(const ReadOptions&,
                                const SliceTransform* prefix_extractor,
                                Arena* arena = nullptr,
                                bool skip_filters = false) override;

  InternalIterator* NewRangeTombstoneIterator(
      const ReadOptions& read_options) override;

  // @param skip_filters Disables loading/accessing the filter block
  Status Get(const ReadOptions& readOptions, const Slice& key,
             GetContext* get_context, const SliceTransform* prefix_extractor,
             bool skip_filters = false) override;
  // HEMANT
  Status GetOriginal(const ReadOptions& readOptions, const Slice& key,
               GetContext* get_context, const SliceTransform* prefix_extractor,
               bool skip_filters = false);

  Status GetColumnOriented(const ReadOptions& readOptions, const Slice& key,
               GetContext* get_context, const SliceTransform* prefix_extractor,
               bool skip_filters = false);
  // HEMANT: END

  // Pre-fetch the disk blocks that correspond to the key range specified by
  // (kbegin, kend). The call will return error status in the event of
  // IO or iteration error.
  Status Prefetch(const Slice* begin, const Slice* end) override;

  // Given a key, return an approximate byte offset in the file where
  // the data for that key begins (or would begin if the key were
  // present in the file).  The returned value is in terms of file
  // bytes, and so includes effects like compression of the underlying data.
  // E.g., the approximate offset of the last key in the table will
  // be close to the file length.
  uint64_t ApproximateOffsetOf(const Slice& key) override;

  // Returns true if the block for the specified key is in cache.
  // REQUIRES: key is in this table && block cache enabled
  bool TEST_KeyInCache(const ReadOptions& options, const Slice& key);

  // Set up the table for Compaction. Might change some parameters with
  // posix_fadvise
  void SetupForCompaction() override;

  std::shared_ptr<const TableProperties> GetTableProperties() const override;

  size_t ApproximateMemoryUsage() const override;

  // convert SST file to a human readable form
  Status DumpTable(WritableFile* out_file,
                   const SliceTransform* prefix_extractor = nullptr) override;

  Status VerifyChecksum() override;

  void Close() override;

  ~BlockBasedTable();

  bool TEST_filter_block_preloaded() const;
  bool TEST_index_reader_preloaded() const;

  // IndexReader is the interface that provide the functionality for index
  // access.
  class IndexReader {
   public:
    explicit IndexReader(const InternalKeyComparator* icomparator,
                         Statistics* stats)
        : icomparator_(icomparator), statistics_(stats) {}

    virtual ~IndexReader() {}

    // Create an iterator for index access.
    // If iter is null then a new object is created on heap and the callee will
    // have the ownership. If a non-null iter is passed in it will be used, and
    // the returned value is either the same as iter or a new on-heap object
    // that
    // wrapps the passed iter. In the latter case the return value would point
    // to
    // a different object then iter and the callee has the ownership of the
    // returned object.
    virtual InternalIterator* NewIterator(BlockIter* iter = nullptr,
                                          bool total_order_seek = true,
                                          bool fill_cache = true) = 0;

    // The size of the index.
    virtual size_t size() const = 0;
    // Memory usage of the index block
    virtual size_t usable_size() const = 0;
    // return the statistics pointer
    virtual Statistics* statistics() const { return statistics_; }
    // Report an approximation of how much memory has been used other than
    // memory
    // that was allocated in block cache.
    virtual size_t ApproximateMemoryUsage() const = 0;

    virtual void CacheDependencies(bool /* unused */) {}

    // Prefetch all the blocks referenced by this index to the buffer
    void PrefetchBlocks(FilePrefetchBuffer* buf);

   protected:
    const InternalKeyComparator* icomparator_;

   private:
    Statistics* statistics_;
  };

  static Slice GetCacheKey(const char* cache_key_prefix,
                           size_t cache_key_prefix_size,
                           const BlockHandle& handle, char* cache_key);

  // Retrieve all key value pairs from data blocks in the table.
  // The key retrieved are internal keys.
  Status GetKVPairsFromDataBlocks(std::vector<KVPairBlock>* kv_pair_blocks);

  template <class TValue>
  struct CachableEntry;
  struct Rep;

  Rep* get_rep() { return rep_; }

  // HEMANT
  void NewDataBlockIterators(Rep* rep, const ReadOptions& read_options,
		  					const std::vector<std::string>& index_values,
                            ColumnIterator& input_iter, uint32_t column_num);

  // input_iter: if it is not null, update this one and return it as Iterator
  static BlockIter* NewDataBlockIterator(Rep* rep, const ReadOptions& ro,
                                         const Slice& index_value,
                                         BlockIter* input_iter = nullptr,
                                         bool is_index = false,
                                         bool key_includes_seq = true,
                                         GetContext* get_context = nullptr);
  static BlockIter* NewDataBlockIterator(Rep* rep, const ReadOptions& ro,
                                         const BlockHandle& block_hanlde,
                                         BlockIter* input_iter = nullptr,
                                         bool is_index = false,
                                         bool key_includes_seq = true,
                                         GetContext* get_context = nullptr,
                                         Status s = Status());
  // Get the iterator from the index reader.
	// If input_iter is not set, return new Iterator
	// If input_iter is set, update it and return it as Iterator
	//
	// Note: ErrorIterator with Status::Incomplete shall be returned if all the
	// following conditions are met:
	//  1. We enabled table_options.cache_index_and_filter_blocks.
	//  2. index is not present in block cache.
	//  3. We disallowed any io to be performed, that is, read_options ==
	//     kBlockCacheTier
	InternalIterator* NewIndexIterator(
		const ReadOptions& read_options, bool prefix_extractor_changed = false,
		BlockIter* input_iter = nullptr,
		CachableEntry<IndexReader>* index_entry = nullptr,
		GetContext* get_context = nullptr);

  class PartitionedIndexIteratorState;

  friend class PartitionIndexReader;

 protected:
  Rep* rep_;
  explicit BlockBasedTable(Rep* rep) : rep_(rep) {}

 private:
  friend class MockedBlockBasedTable;
  static std::atomic<uint64_t> next_cache_key_id_;

  // If block cache enabled (compressed or uncompressed), looks for the block
  // identified by handle in (1) uncompressed cache, (2) compressed cache, and
  // then (3) file. If found, inserts into the cache(s) that were searched
  // unsuccessfully (e.g., if found in file, will add to both uncompressed and
  // compressed caches if they're enabled).
  //
  // @param block_entry value is set to the uncompressed block if found. If
  //    in uncompressed block cache, also sets cache_handle to reference that
  //    block.
  static Status MaybeLoadDataBlockToCache(FilePrefetchBuffer* prefetch_buffer,
                                          Rep* rep, const ReadOptions& ro,
                                          const BlockHandle& handle,
                                          Slice compression_dict,
                                          CachableEntry<Block>* block_entry,
                                          bool is_index = false,
                                          GetContext* get_context = nullptr);

  // For the following two functions:
  // if `no_io == true`, we will not try to read filter/index from sst file
  // were they not present in cache yet.
  CachableEntry<FilterBlockReader> GetFilter(
      const SliceTransform* prefix_extractor = nullptr,
      FilePrefetchBuffer* prefetch_buffer = nullptr, bool no_io = false,
      GetContext* get_context = nullptr) const;
  virtual CachableEntry<FilterBlockReader> GetFilter(
      FilePrefetchBuffer* prefetch_buffer, const BlockHandle& filter_blk_handle,
      const bool is_a_filter_partition, bool no_io, GetContext* get_context,
      const SliceTransform* prefix_extractor = nullptr) const;

  // HEMANT
  void NewIndexIterators(const ReadOptions& read_options,
						bool disable_prefix_seek,
						GetContext* get_context,
						ColumnIterator& input_iter,
						uint32_t column_num);


  // Read block cache from block caches (if set): block_cache and
  // block_cache_compressed.
  // On success, Status::OK with be returned and @block will be populated with
  // pointer to the block as well as its block handle.
  // @param compression_dict Data for presetting the compression library's
  //    dictionary.
  static Status GetDataBlockFromCache(
      const Slice& block_cache_key, const Slice& compressed_block_cache_key,
      Cache* block_cache, Cache* block_cache_compressed,
      const ImmutableCFOptions& ioptions, const ReadOptions& read_options,
      BlockBasedTable::CachableEntry<Block>* block, uint32_t format_version,
      const Slice& compression_dict, size_t read_amp_bytes_per_bit,
      bool is_index = false, GetContext* get_context = nullptr);

  // Put a raw block (maybe compressed) to the corresponding block caches.
  // This method will perform decompression against raw_block if needed and then
  // populate the block caches.
  // On success, Status::OK will be returned; also @block will be populated with
  // uncompressed block and its cache handle.
  //
  // REQUIRES: raw_block is heap-allocated. PutDataBlockToCache() will be
  // responsible for releasing its memory if error occurs.
  // @param compression_dict Data for presetting the compression library's
  //    dictionary.
  static Status PutDataBlockToCache(
      const Slice& block_cache_key, const Slice& compressed_block_cache_key,
      Cache* block_cache, Cache* block_cache_compressed,
      const ReadOptions& read_options, const ImmutableCFOptions& ioptions,
      CachableEntry<Block>* block, Block* raw_block, uint32_t format_version,
      const Slice& compression_dict, size_t read_amp_bytes_per_bit,
      bool is_index = false, Cache::Priority pri = Cache::Priority::LOW,
      GetContext* get_context = nullptr);

  // Calls (*handle_result)(arg, ...) repeatedly, starting with the entry found
  // after a call to Seek(key), until handle_result returns false.
  // May not make such a call if filter policy says that key is not present.
  friend class TableCache;
  friend class BlockBasedTableBuilder;

  void ReadMeta(const Footer& footer);

  // Create a index reader based on the index type stored in the table.
  // Optionally, user can pass a preloaded meta_index_iter for the index that
  // need to access extra meta blocks for index construction. This parameter
  // helps avoid re-reading meta index block if caller already created one.
  Status CreateIndexReader(
      FilePrefetchBuffer* prefetch_buffer, IndexReader** index_reader,
      InternalIterator* preloaded_meta_index_iter = nullptr,
      const int level = -1);

  bool FullFilterKeyMayMatch(
      const ReadOptions& read_options, FilterBlockReader* filter,
      const Slice& user_key, const bool no_io,
      const SliceTransform* prefix_extractor = nullptr) const;

  // Read the meta block from sst.
  static Status ReadMetaBlock(Rep* rep, FilePrefetchBuffer* prefetch_buffer,
                              std::unique_ptr<Block>* meta_block,
                              std::unique_ptr<InternalIterator>* iter);

  Status VerifyChecksumInBlocks(InternalIterator* index_iter);

  // Create the filter from the filter block.
  virtual FilterBlockReader* ReadFilter(
      FilePrefetchBuffer* prefetch_buffer, const BlockHandle& filter_handle,
      const bool is_a_filter_partition,
      const SliceTransform* prefix_extractor = nullptr) const;

  static void SetupCacheKeyPrefix(Rep* rep, uint64_t file_size);

  // Generate a cache key prefix from the file
  static void GenerateCachePrefix(Cache* cc,
    RandomAccessFile* file, char* buffer, size_t* size);
  static void GenerateCachePrefix(Cache* cc,
    WritableFile* file, char* buffer, size_t* size);

  // Helper functions for DumpTable()
  Status DumpIndexBlock(WritableFile* out_file);
  Status DumpDataBlocks(WritableFile* out_file);
  void DumpKeyValue(const Slice& key, const Slice& value,
                    WritableFile* out_file);

  // No copying allowed
  explicit BlockBasedTable(const TableReader&) = delete;
  void operator=(const TableReader&) = delete;

  friend class PartitionedFilterBlockReader;
  friend class PartitionedFilterBlockTest;
};

// Maitaning state of a two-level iteration on a partitioned index structure
class BlockBasedTable::PartitionedIndexIteratorState
    : public TwoLevelIteratorState {
 public:
  PartitionedIndexIteratorState(
      BlockBasedTable* table,
      std::unordered_map<uint64_t, CachableEntry<Block>>* block_map,
      const bool index_key_includes_seq);
  InternalIterator* NewSecondaryIterator(const Slice& index_value) override;

 private:
  // Don't own table_
  BlockBasedTable* table_;
  std::unordered_map<uint64_t, CachableEntry<Block>>* block_map_;
  bool index_key_includes_seq_;
};

// CachableEntry represents the entries that *may* be fetched from block cache.
//  field `value` is the item we want to get.
//  field `cache_handle` is the cache handle to the block cache. If the value
//    was not read from cache, `cache_handle` will be nullptr.
template <class TValue>
struct BlockBasedTable::CachableEntry {
  CachableEntry(TValue* _value, Cache::Handle* _cache_handle)
      : value(_value), cache_handle(_cache_handle) {}
  CachableEntry() : CachableEntry(nullptr, nullptr) {}
  void Release(Cache* cache, bool force_erase = false) {
    if (cache_handle) {
      cache->Release(cache_handle, force_erase);
      value = nullptr;
      cache_handle = nullptr;
    }
  }
  bool IsSet() const { return cache_handle != nullptr; }

  TValue* value = nullptr;
  // if the entry is from the cache, cache_handle will be populated.
  Cache::Handle* cache_handle = nullptr;
};

struct BlockBasedTable::Rep {
  Rep(const ImmutableCFOptions& _ioptions, const EnvOptions& _env_options,
      const BlockBasedTableOptions& _table_opt,
      const InternalKeyComparator& _internal_comparator, bool skip_filters)
      : ioptions(_ioptions),
        env_options(_env_options),
        table_options(_table_opt),
        filter_policy(skip_filters ? nullptr : _table_opt.filter_policy.get()),
        internal_comparator(_internal_comparator),
        filter_type(FilterType::kNoFilter),
        index_type(BlockBasedTableOptions::IndexType::kBinarySearch),
        hash_index_allow_collision(false),
        whole_key_filtering(_table_opt.whole_key_filtering),
        prefix_filtering(true),
        range_del_handle(BlockHandle::NullBlockHandle()),
        global_seqno(kDisableGlobalSequenceNumber) {}

  const ImmutableCFOptions& ioptions;
  const EnvOptions& env_options;
  const BlockBasedTableOptions table_options;
  const FilterPolicy* const filter_policy;
  const InternalKeyComparator& internal_comparator;
  Status status;
  unique_ptr<RandomAccessFileReader> file;
  char cache_key_prefix[kMaxCacheKeyPrefixSize];
  size_t cache_key_prefix_size = 0;
  char persistent_cache_key_prefix[kMaxCacheKeyPrefixSize];
  size_t persistent_cache_key_prefix_size = 0;
  char compressed_cache_key_prefix[kMaxCacheKeyPrefixSize];
  size_t compressed_cache_key_prefix_size = 0;
  uint64_t dummy_index_reader_offset =
      0;  // ID that is unique for the block cache.
  PersistentCacheOptions persistent_cache_options;

  // Footer contains the fixed table information
  Footer footer;
  // index_reader and filter will be populated and used only when
  // options.block_cache is nullptr; otherwise we will get the index block via
  // the block cache.
  unique_ptr<IndexReader> index_reader;
  unique_ptr<FilterBlockReader> filter;

  enum class FilterType {
    kNoFilter,
    kFullFilter,
    kBlockFilter,
    kPartitionedFilter,
  };
  FilterType filter_type;
  BlockHandle filter_handle;

  std::shared_ptr<const TableProperties> table_properties;
  // Block containing the data for the compression dictionary. We take ownership
  // for the entire block struct, even though we only use its Slice member. This
  // is easier because the Slice member depends on the continued existence of
  // another member ("allocation").
  std::unique_ptr<const BlockContents> compression_dict_block;
  BlockBasedTableOptions::IndexType index_type;
  bool hash_index_allow_collision;
  bool whole_key_filtering;
  bool prefix_filtering;
  // TODO(kailiu) It is very ugly to use internal key in table, since table
  // module should not be relying on db module. However to make things easier
  // and compatible with existing code, we introduce a wrapper that allows
  // block to extract prefix without knowing if a key is internal or not.
  unique_ptr<SliceTransform> internal_prefix_transform;

  // only used in level 0 files:
  // when pin_l0_filter_and_index_blocks_in_cache is true, we do use the
  // LRU cache, but we always keep the filter & idndex block's handle checked
  // out here (=we don't call Release()), plus the parsed out objects
  // the LRU cache will never push flush them out, hence they're pinned
  CachableEntry<FilterBlockReader> filter_entry;
  CachableEntry<IndexReader> index_entry;
  // range deletion meta-block is pinned through reader's lifetime when LRU
  // cache is enabled.
  CachableEntry<Block> range_del_entry;
  BlockHandle range_del_handle;

  // If global_seqno is used, all Keys in this file will have the same
  // seqno with value `global_seqno`.
  //
  // A value of kDisableGlobalSequenceNumber means that this feature is disabled
  // and every key have it's own seqno.
  SequenceNumber global_seqno;

  // If false, blocks in this file are definitely all uncompressed. Knowing this
  // before reading individual blocks enables certain optimizations.
  bool blocks_maybe_compressed = true;

  bool closed = false;
  bool is_column_oriented = false; // HEMANT: to keep track if column oriented reader
  std::vector<unique_ptr<BlockBasedTable>> tables; // HEMANT: maintain a list of column readers
  int level = -1; // HEMANT: (UPDATE for CGs) nee this to figure out columns in a CG inside get_context.cc
};

class BlockBasedTableIterator : public InternalIterator {
 public:
  BlockBasedTableIterator(BlockBasedTable* table,
                          const ReadOptions& read_options,
                          const InternalKeyComparator& icomp,
                          InternalIterator* index_iter, bool check_filter,
                          const SliceTransform* prefix_extractor, bool is_index,
                          bool key_includes_seq = true)
      : table_(table),
        read_options_(read_options),
        icomp_(icomp),
        index_iter_(index_iter),
        pinned_iters_mgr_(nullptr),
        block_iter_points_to_real_block_(false),
        check_filter_(check_filter),
        is_index_(is_index),
        key_includes_seq_(key_includes_seq),
        prefix_extractor_(prefix_extractor) {}

  ~BlockBasedTableIterator() { delete index_iter_; }

  Status RangeQuery(const LookupRange& range,
					  /*std::map<std::string, SeqTypeVal>&*/int* res
					  /*filterFun filter,
					  groupFun group,
					  void* arg,
					  bool unique_key*/) /*const*/ override;
  void Seek(const Slice& target) override;
  void SeekForPrev(const Slice& target) override;
  void SeekToFirst() override;
  void SeekToLast() override;
  void Next() override;
  void Prev() override;
  bool Valid() const override {
    return !is_out_of_bound_ && block_iter_points_to_real_block_ &&
           data_block_iter_.Valid();
  }
  Slice key() const override {
    assert(Valid());
    return data_block_iter_.key();
  }
  Slice value() const override {
    assert(Valid());
    return data_block_iter_.value();
  }
  Status status() const override {
    if (!index_iter_->status().ok()) {
      return index_iter_->status();
    } else if (block_iter_points_to_real_block_) {
      return data_block_iter_.status();
    } else {
      return Status::OK();
    }
  }

  bool IsOutOfBound() override { return is_out_of_bound_; }

  void SetPinnedItersMgr(PinnedIteratorsManager* pinned_iters_mgr) override {
    pinned_iters_mgr_ = pinned_iters_mgr;
  }
  bool IsKeyPinned() const override {
    return pinned_iters_mgr_ && pinned_iters_mgr_->PinningEnabled() &&
           block_iter_points_to_real_block_ && data_block_iter_.IsKeyPinned();
  }
  bool IsValuePinned() const override {
    // BlockIter::IsValuePinned() is always true. No need to check
    return pinned_iters_mgr_ && pinned_iters_mgr_->PinningEnabled() &&
           block_iter_points_to_real_block_;
  }

  bool CheckPrefixMayMatch(const Slice& ikey,
                           const SliceTransform* prefix_extractor = nullptr) {
    if (check_filter_ && !table_->PrefixMayMatch(ikey, prefix_extractor)) {
      // TODO remember the iterator is invalidated because of prefix
      // match. This can avoid the upper level file iterator to falsely
      // believe the position is the end of the SST file and move to
      // the first key of the next file.
      ResetDataIter();
      return false;
    }
    return true;
  }

  void ResetDataIter() {
    if (block_iter_points_to_real_block_) {
      if (pinned_iters_mgr_ != nullptr && pinned_iters_mgr_->PinningEnabled()) {
        data_block_iter_.DelegateCleanupsTo(pinned_iters_mgr_);
      }
      data_block_iter_.Invalidate(Status::OK());
      block_iter_points_to_real_block_ = false;
    }
  }

  void SavePrevIndexValue() {
    if (block_iter_points_to_real_block_) {
      // Reseek. If they end up with the same data block, we shouldn't re-fetch
      // the same data block.
      Slice v = index_iter_->value();
      prev_index_value_.assign(v.data(), v.size());
    }
  }

  void InitDataBlock();
  void FindKeyForward();
  void FindKeyBackward();

 private:
  BlockBasedTable* table_;
  const ReadOptions read_options_;
  const InternalKeyComparator& icomp_;
  InternalIterator* index_iter_;
  PinnedIteratorsManager* pinned_iters_mgr_;
  BlockIter data_block_iter_;
  bool block_iter_points_to_real_block_;
  bool is_out_of_bound_ = false;
  bool check_filter_;
  // If the blocks over which we iterate are index blocks
  bool is_index_;
  // If the keys in the blocks over which we iterate include 8 byte sequence
  bool key_includes_seq_;
  // TODO use block offset instead
  std::string prev_index_value_;
  const SliceTransform* prefix_extractor_;

  static const size_t kInitReadaheadSize = 8 * 1024;
  // Found that 256 KB readahead size provides the best performance, based on
  // experiments.
  static const size_t kMaxReadaheadSize;
  size_t readahead_size_ = kInitReadaheadSize;
  size_t readahead_limit_ = 0;
  int num_file_reads_ = 0;
};


// HEMANT: Class to iterate through column files
class ColumnIterator : public InternalIterator {
 public:
  ColumnIterator(const std::vector<InternalIterator*>& columns,
                 char delim,
                 const InternalKeyComparator& internal_comparator,
                 uint64_t num_entries = 0,
                 std::vector<uint32_t> filter_columns = {},
                 std::vector<char> filter_types = {},
                 std::vector<std::string> filter_values = {},
                 std::vector<std::string> filter_comparator = {},
                 std::vector<size_t> col_attrs_offset = {},
                 std::vector<size_t> col_attrs_size = {})
  : columns_(columns),
    delim_(delim),
    internal_comparator_(internal_comparator),
    num_entries_(num_entries),
    filter_columns_(filter_columns),
    filter_types_(filter_types),
    filter_values_(filter_values),
    filter_comparator_(filter_comparator),
    col_attrs_offset_(col_attrs_offset),
    col_attrs_size_(col_attrs_size){}

  virtual ~ColumnIterator() {
    for (const auto& it : columns_) {
      it->~InternalIterator();
    }
  }

  virtual bool Valid() const {
    for (const auto& it : columns_) {
      if (!it->Valid()) {
        return false;
      }
    }
    return true;
  }

  virtual void SeekToFirst() {
    for (const auto& it : columns_) {
      it->SeekToFirst();
    }
    ParseCurrentValue();
  }

  virtual void SeekToLast() {
    for (const auto& it : columns_) {
      it->SeekToLast();
    }
    ParseCurrentValue();
  }

  virtual void Seek(const Slice& target) {
    for (const auto& it : columns_) {
      it->Seek(target);
    }
    ParseCurrentValue();
  }

  void SeekForPrev(const Slice& target) {
	  for (const auto& it : columns_) {
		  it->SeekForPrev(target);
	  }
	  ParseCurrentValue();
  }

  virtual void Next() {
    assert(Valid());
    for (const auto& it : columns_) {
      it->Next();
    }
    ParseCurrentValue();
  }

  virtual void Prev() {
    assert(Valid());
    for (const auto& it : columns_) {
      it->Prev();
    }
    ParseCurrentValue();
  }

  virtual Slice key() const {
    assert(Valid());
    return columns_[0]->key();
  }

  virtual Slice value() const {
    assert(Valid());
    return value_;
  }

  virtual Status status() const {
    if (!status_.ok()) {
      return status_;
    }
    Status s;
    for (const auto& it : columns_) {
      s = it->status();
      if (!s.ok()) {
        break;
      }
    }
    return s;
  }


  virtual Status RangeQuery(const LookupRange& range,
                            /*std::map<std::string, SeqTypeVal>&*/int* res/*,
                            filterFun filter,
                            groupFun group,
                            void* arg,
                            bool unique_key*/) /*const*/ {
    SequenceNumber sequence_num = range.SequenceNum();
    //std::vector<Slice> tmp_keys;
    //std::vector<std::vector<Slice>> tmp_values;

    /*if (num_entries_ > 0) {
      tmp_keys.reserve(num_entries_);
      tmp_values.reserve(num_entries_);
    }*/
    int count = 0;
    for (auto i = 0u; i < columns_.size(); i++) {
      InternalIterator* it = columns_[i];
      std::string val, user_key;
      //std::map<uint32_t, const std::string*> v;
      if (i == 0) {
		  for (it->Seek(range.start_->internal_key()); it->Valid(); it->Next()) {
			  if (internal_comparator_.Compare(it->key(), range.limit_->internal_key()) >= 0) {
				  break;
			  }
			  ParsedInternalKey parsed_key;
			  if (!ParseInternalKey(it->key(), &parsed_key)) {
				  return Status::Corruption("corrupted internal key in Table::Iter");
			  }
			  if (parsed_key.sequence <= sequence_num && parsed_key.type != kTypeMerge) {
				  //tmp_keys.push_back(it->key());
				  //std::vector<Slice> value_vec;
				  //value_vec.push_back(it->value());
				  //tmp_values.push_back(value_vec);
				  if(filter_values_.size() > 0) {
					  bool filter_pass = true;
					  uint32_t col_index = filter_columns_[i]-1;
					  size_t attr_size = col_attrs_size_[col_index];
					  char type = filter_types_[i];
					  char c_val[attr_size];
					  memcpy(c_val, it->value().data_, attr_size);
					  switch(type) {
						case 'D' :
						{
						  double filter = std::stod(filter_values_[i]);
						  double value = std::atof(c_val);
						  if(filter_comparator_[i] == ">") {
							  if(value <= filter)
								  filter_pass = false;
						  }
						  break;
						}
						default :
						  continue;
					  }
					  if (!filter_pass) {
						continue;
					  }
					  count++;
				  }
				  //val.assign(it->value().data(), it->value().size());
				  //user_key.assign(it->key().data(), it->key().size() - 8);
				  //res.emplace(user_key, SeqTypeVal(parsed_key.sequence, parsed_key.type, val));
			  }
		  }
      } else {
    	  for (it->Seek(range.start_->internal_key()); it->Valid(); it->Next()) {
    		  if (internal_comparator_.Compare(it->key(), range.limit_->internal_key()) >= 0) {
				  break;
			  }
			  ParsedInternalKey parsed_key;
			  if (!ParseInternalKey(it->key(), &parsed_key)) {
				  return Status::Corruption("corrupted internal key in Table::Iter");
			  }
			  if (parsed_key.sequence <= sequence_num && parsed_key.type != kTypeMerge) {
				  //val.assign(it->value().data(), it->value().size());
				  //tmp_values[inn_cnt++].push_back(it->value());
			  }
    	  }
      }
    }

    //assert(tmp_keys.size() == tmp_STVs.size());
    //auto hint = res.end();
    /*for (auto i = 0u; i < tmp_keys.size(); i++) {
    	//res.emplace(tmp_keys[i], tmp_STVs[i]);
    	//std::vector<const std::string*> v{&tmp_keys[i], &tmp_STVs[i].val_};
    	bool filter_pass = true;
		for(uint32_t j = 0; j < filter_columns_.size(); j++) {
			uint32_t col_index = filter_columns_[j]-1;
			size_t attr_size = col_attrs_size_[col_index];
			//size_t attr_offset = col_attrs_offset_[col_index];
			char type = filter_types_[j];
			char c_val[attr_size];
			memcpy(c_val, tmp_values[i][j].data_, attr_size);
			switch(type) {
				case 'D' :
				{
				  double filter = std::stod(filter_values_[j]);
				  double value = std::atof(c_val);
				  if(filter_comparator_[j] == ">") {
					  if(value <= filter)
						  filter_pass = false;
				  }
				  break;
				}
				default :
				  continue;
			}
		}
		if (!filter_pass) {
			continue;
		}
    }*/
    *res = *res + count;
    return Status();
  }

  virtual void SetPinnedItersMgr(PinnedIteratorsManager* pinned_iters_mgr) {
    for (const auto& it : columns_) {
      it->SetPinnedItersMgr(pinned_iters_mgr);
    }
  }

  virtual bool IsKeyPinned() const {
    for (const auto& it : columns_) {
      if (!it->IsKeyPinned()) {
        return false;
      }
    }
    return true;
  }

  bool SetIter(uint32_t i, InternalIterator* const it) {
    if (i < columns_.size()) {
      columns_[i] = it;
      return true;
    } else {
      return false;
    }
  }

  const std::vector<std::string>& GetVals() const { return vals_; }

 private:
  inline bool ParseCurrentValue() {
    value_.clear();
    vals_.clear();
    for (auto i = 0u; i < columns_.size(); i++) {
      if (!columns_[i]->Valid()) {
        return false;
      }
      vals_.emplace_back(columns_[i]->value().ToString());
      value_ += vals_.back();
      if (i < columns_.size() - 1) {
        value_ += delim_;
      }
    }
    return true;
  }

  std::vector<InternalIterator*> columns_;
  std::string value_;
  char delim_;
  Status status_;
  const InternalKeyComparator& internal_comparator_;
  uint64_t num_entries_;
  std::vector<uint32_t> filter_columns_;
  std::vector<std::string> vals_;
  std::vector<char> filter_types_; //  types for filter. "D" for Double, and so on
  std::vector<std::string> filter_values_;  //  filter value. E.g. 0.01
  std::vector<std::string> filter_comparator_; //  comparator ">" or "<" or ">=" and so on
  std::vector<size_t> col_attrs_offset_; // HEMANT: Vector to hold the offset value of each attribute
  std::vector<size_t> col_attrs_size_;
};
// HEMANT: END
}  // namespace rocksdb
