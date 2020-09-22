/**
 * A MergeOperator for kLSM-rocksdb that implements merging column updates.
 * @author Hemant Saxena (h2saxena@uwaterloo.ca)
 */

#pragma once
#include "rocksdb/merge_operator.h"
#include "rocksdb/slice.h"
#include <vector>

namespace rocksdb {

class ColumnMerger : public AssociativeMergeOperator {
  public:

    explicit ColumnMerger(char delim_char, uint32_t num_columns);

    virtual bool Merge(const Slice& key,
                  const Slice* existing_value,
                  const Slice& value,
                  std::string* new_value,
                  Logger* logger) const override;

    virtual const char* Name() const override;

  private:
    char delim_;
    uint32_t num_columns_;    
};

}