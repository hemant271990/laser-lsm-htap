/**
 * A MergeOperator for kLSM-rocksdb that implements merging column updates.
 * @author Hemant Saxena (h2saxena@uwaterloo.ca)
 */

#include "column_merger.h"

#include <assert.h>

#include "rocksdb/slice.h"
#include "rocksdb/merge_operator.h"
#include <vector>

namespace rocksdb {

  // Constructor: also specify the delimiter character, and initiating splits vector.
  ColumnMerger::ColumnMerger(char delim_char, uint32_t num_columns)
      : delim_(delim_char),
        num_columns_(num_columns) {
  }

  // Implementation for the merge operation.
  // keep only latest updates per column
  bool ColumnMerger::Merge(const Slice& /*key*/,
                                  const Slice* existing_value,
                                  const Slice& value, std::string* new_value,
                                  Logger* /*logger*/) const {
    // Clear the *new_value for writing.
    assert(new_value);
    assert(&value);

    if (!existing_value) {
      // No existing_value. Set *new_value = value
      new_value->assign(value.data(),value.size());
      return true;
    }
    new_value->clear();
    size_t old_ptr = 0;
    size_t new_ptr = 0;
    uint32_t cols_checked = 0;
    size_t old_size = 0;
    size_t new_size = 0;

/* #ifndef NDEBUG
    std::vector<std::string> vals = existing_value->split(delim_);
    std::vector<std::string> vals2 = value.split(delim_);
    // Two values should always be aligned
    assert(vals.size() == vals2.size());
#endif */

    while(cols_checked < num_columns_ && old_ptr < existing_value->size() && new_ptr < value.size()) {
      // collect old value
      while (old_ptr < existing_value->size() && existing_value->data()[old_ptr] != delim_) {
        old_size++;
        old_ptr++;
      }

      // collect new value
      while (new_ptr < value.size() && value.data()[new_ptr] != delim_) {
        new_size++;
        new_ptr++;
      }

      // append new if non-empty, else append old
      if (new_size > 0) {
        new_value->append(value.data() + new_ptr - new_size, new_size);
      } else if (old_size > 0) {
        new_value->append(existing_value->data() + old_ptr - old_size, old_size);
      }

      // append delim only if this is delim seperated column values
      // single column merges are not delim seperated so we shoudn't append delim
      if(existing_value->data()[old_ptr] == delim_ && value.data()[new_ptr] == delim_) {
        new_value->append(1, delim_);
      }

      // increase new and old ptr if they were empty
      new_ptr++;
      old_ptr++;

      old_size = 0;
      new_size = 0;
      cols_checked++;
    }

    // Merge should be called either on completely aligned columns (in the case of row levels), 
    // or just for single columns (in the case of compaction)
    // assert(cols_checked == num_columns_ || cols_checked == 1);
    return true;
  }

  const char* ColumnMerger::Name() const  {
    return "ColumnMerger";
  }

}