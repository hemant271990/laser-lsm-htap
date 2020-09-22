//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2012 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <algorithm>
#include "rocksdb/slice_transform.h"
#include "rocksdb/slice.h"
#include "util/string_util.h"
#include <stdio.h>

namespace rocksdb {

namespace {

class FixedPrefixTransform : public SliceTransform {
 private:
  size_t prefix_len_;
  std::string name_;

 public:
  explicit FixedPrefixTransform(size_t prefix_len)
      : prefix_len_(prefix_len),
        // Note that if any part of the name format changes, it will require
        // changes on options_helper in order to make RocksDBOptionsParser work
        // for the new change.
        // TODO(yhchiang): move serialization / deserializaion code inside
        // the class implementation itself.
        name_("rocksdb.FixedPrefix." + ToString(prefix_len_)) {}

  virtual const char* Name() const override { return name_.c_str(); }

  virtual Slice Transform(const Slice& src) const override {
    assert(InDomain(src));
    return Slice(src.data(), prefix_len_);
  }

  virtual bool InDomain(const Slice& src) const override {
    return (src.size() >= prefix_len_);
  }

  virtual bool InRange(const Slice& dst) const override {
    return (dst.size() == prefix_len_);
  }

  virtual bool SameResultWhenAppended(const Slice& prefix) const override {
    return InDomain(prefix);
  }
};

class CappedPrefixTransform : public SliceTransform {
 private:
  size_t cap_len_;
  std::string name_;

 public:
  explicit CappedPrefixTransform(size_t cap_len)
      : cap_len_(cap_len),
        // Note that if any part of the name format changes, it will require
        // changes on options_helper in order to make RocksDBOptionsParser work
        // for the new change.
        // TODO(yhchiang): move serialization / deserializaion code inside
        // the class implementation itself.
        name_("rocksdb.CappedPrefix." + ToString(cap_len_)) {}

  virtual const char* Name() const override { return name_.c_str(); }

  virtual Slice Transform(const Slice& src) const override {
    assert(InDomain(src));
    return Slice(src.data(), std::min(cap_len_, src.size()));
  }

  virtual bool InDomain(const Slice& /*src*/) const override { return true; }

  virtual bool InRange(const Slice& dst) const override {
    return (dst.size() <= cap_len_);
  }

  virtual bool SameResultWhenAppended(const Slice& prefix) const override {
    return prefix.size() >= cap_len_;
  }
};

class NoopTransform : public SliceTransform {
 public:
  explicit NoopTransform() { }

  virtual const char* Name() const override { return "rocksdb.Noop"; }

  virtual Slice Transform(const Slice& src) const override { return src; }

  virtual bool InDomain(const Slice& /*src*/) const override { return true; }

  virtual bool InRange(const Slice& /*dst*/) const override { return true; }

  virtual bool SameResultWhenAppended(const Slice& /*prefix*/) const override {
    return false;
  }
};

}

// 2 small internal utility functions, for efficient hex conversions
// and no need for snprintf, toupper etc...
// Originally from wdt/util/EncryptionUtils.cpp - for ToString(true)/DecodeHex:
char toHex(unsigned char v) {
  if (v <= 9) {
    return '0' + v;
  }
  return 'A' + v - 10;
}
// most of the code is for validation/error check
int fromHex(char c) {
  // toupper:
  if (c >= 'a' && c <= 'f') {
    c -= ('a' - 'A');  // aka 0x20
  }
  // validation
  if (c < '0' || (c > '9' && (c < 'A' || c > 'F'))) {
    return -1;  // invalid not 0-9A-F hex char
  }
  if (c <= '9') {
    return c - '0';
  }
  return c - 'A' + 10;
}

Slice::Slice(const SliceParts& parts, std::string* buf) {
  size_t length = 0;
  for (int i = 0; i < parts.num_parts; ++i) {
    length += parts.parts[i].size();
  }
  buf->reserve(length);

  for (int i = 0; i < parts.num_parts; ++i) {
    buf->append(parts.parts[i].data(), parts.parts[i].size());
  }
  data_ = buf->data();
  size_ = buf->size();
}

// Return a string that contains the copy of the referenced data.
std::string Slice::ToString(bool hex) const {
  std::string result;  // RVO/NRVO/move
  if (hex) {
    result.reserve(2 * size_);
    for (size_t i = 0; i < size_; ++i) {
      unsigned char c = data_[i];
      result.push_back(toHex(c >> 4));
      result.push_back(toHex(c & 0xf));
    }
    return result;
  } else {
    result.assign(data_, size_);
    return result;
  }
}

// HEMANT: split slice by delim. Use for user_key or value. 
// won't work with internal key because it stores metadata too
std::vector<std::string> Slice::split(char delim) const {
  std::vector<std::string> splits;
  std::string tmp = "";
  for (size_t i = 0; i < size_; ++i) {
    if (data_[i] != delim) {
      tmp += data_[i];
    } else {
      splits.push_back(tmp);
      tmp = "";
    }
  }
  if(tmp != "") {
    splits.push_back(tmp);
  }
  return splits;
}

// HEMANT: split slice by delim. Use for user_key or value. 
// won't work with internal key because it stores metadata too
void Slice::split(char delim, std::vector<std::string>& splits) const {
  size_t tmp_size = 0;
  size_t count = 0;
  size_t i = 0;
  for (i = 0; i < size_; ++i) {
    if (data_[i] != delim) {
      tmp_size++;
    } else {
      splits[count].assign(data_ + i - tmp_size, tmp_size);
       tmp_size = 0;
      count++;
    }
  }
  if(tmp_size > 0) {
    splits[count].assign(data_ + i - tmp_size, tmp_size);
    count++;
  }
  // assert that we tried spliting value to all elements of splits
  assert(count == splits.size());
}

// HEMANT: split slice by column groups. Use for user_key or value. 
// won't work with internal key because it stores metadata too
void Slice::splitByCGs(char delim, std::vector<std::string>& splits, const std::vector<std::tuple<uint32_t, uint32_t>>& CG) const {
  assert(splits.size() == CG.size());
  size_t tmp_size = 0;
  size_t count = 0;
  size_t i = 0;
  uint32_t col = std::get<0>(CG[0]); // marked as first column in this vec of CGs
  size_t curr_cg = 0;
  for (i = 0; i < size_; ++i) {
    if (data_[i] == delim) {
      col++;
      tmp_size++;
      if (col-1 == std::get<1>(CG[curr_cg])) {
        // if this delim marks end of a CG, assign to splits
        splits[count].assign(data_ + i+1 - tmp_size, tmp_size);
        count++;
        curr_cg++; // move to next cg
        tmp_size = 0; // reset tmp_size
      }
    } else {
      tmp_size++;
    }
  }
  assert(tmp_size == 0); // assert that we don't leave any content unassigned  
  assert(count == CG.size()); // assert that we filled up values for all CGs
}

// HEMANT: split slice by delim, and merge with splits vector. Use for user_key or value. 
// won't work with internal key because it stores metadata too
void Slice::splitAndMergeTo(char delim, std::vector<std::string>& splits, std::vector<uint32_t>& filter_columns) const {
  size_t tmp_size = 0;
  size_t count = 0; // keeps track of which column value are we at
  size_t filter_idx = 0;
  size_t i = 0;
  for (i = 0; i < size_; ++i) {
    if (data_[i] != delim) {
      tmp_size++;
    } else {
      // check if filter allows this value, check if value is new
      if(filter_columns.at(filter_idx)-1 == count) {
        if(splits.at(filter_idx).size() == 0) {
          splits.at(filter_idx).assign(data_ + i - tmp_size, tmp_size);
        }
        filter_idx++;
	if(filter_idx >= filter_columns.size()) {
          break;
        }
      }
      tmp_size = 0;
      count++;
    }
  }
  if(tmp_size > 0 && filter_idx < filter_columns.size()) {
    if(filter_columns.at(filter_idx)-1 == count && splits.at(filter_idx).size() == 0) {
      splits.at(filter_idx).assign(data_ + i - tmp_size, tmp_size);
      filter_idx++;
    }
    count++;
  }
  // assert that we tried merging value to all elements of splits
  assert(filter_idx == splits.size());
}

// HEMANT: split slice by delim, and merge with splits vector. Use for user_key or value. 
// won't work with internal key because it stores metadata too
void Slice::splitAndMergeTo(char delim, std::vector<std::string>& splits) const {
  size_t tmp_size = 0;
  size_t count = 0; // keeps track of which column value are we at
  size_t i = 0;
  for (i = 0; i < size_; ++i) {
    if (data_[i] != delim) {
      tmp_size++;
    } else {
      if(splits[count].size() == 0) {
        splits[count].assign(data_ + i - tmp_size, tmp_size);
      }
      tmp_size = 0;
      count++;
    }
  }
  if(tmp_size > 0) {
    if(splits[count].size() == 0) {
      splits[count].assign(data_ + i - tmp_size, tmp_size);
    }
    count++;
  }
  // assert that we tried merging value to all elements of splits
  assert(count == splits.size());
}

// Originally from rocksdb/utilities/ldb_cmd.h
bool Slice::DecodeHex(std::string* result) const {
  std::string::size_type len = size_;
  if (len % 2) {
    // Hex string must be even number of hex digits to get complete bytes back
    return false;
  }
  if (!result) {
    return false;
  }
  result->clear();
  result->reserve(len / 2);

  for (size_t i = 0; i < len;) {
    int h1 = fromHex(data_[i++]);
    if (h1 < 0) {
      return false;
    }
    int h2 = fromHex(data_[i++]);
    if (h2 < 0) {
      return false;
    }
    result->push_back(static_cast<char>((h1 << 4) | h2));
  }
  return true;
}

const SliceTransform* NewFixedPrefixTransform(size_t prefix_len) {
  return new FixedPrefixTransform(prefix_len);
}

const SliceTransform* NewCappedPrefixTransform(size_t cap_len) {
  return new CappedPrefixTransform(cap_len);
}

const SliceTransform* NewNoopTransform() {
  return new NoopTransform;
}

}  // namespace rocksdb
