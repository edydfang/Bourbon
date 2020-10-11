// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_TABLE_BLOCK_H_
#define STORAGE_LEVELDB_TABLE_BLOCK_H_

#include <stddef.h>
#include <stdint.h>
#include <util/coding.h>

#include "leveldb/iterator.h"
#include "leveldb/comparator.h"
#include "table/format.h"
#include "mod/util.h"

namespace leveldb {

struct BlockContents;

class Comparator;

class Block {
public:
    // Initialize the block with the specified contents.
    explicit Block(const BlockContents &contents);

    Block(const Block &) = delete;

    Block &operator=(const Block &) = delete;

    ~Block();

    size_t size() const { return size_; }

    Iterator *NewIterator(const Comparator *comparator);


private:
    friend class Table;
    friend class TableCache;

    class Iter;

    uint32_t NumRestarts() const;

    const char *data_;
    size_t size_;
    uint32_t restart_offset_;  // Offset in data_ of restart array
    bool owned_;               // Block owns data_[]
};

// Helper routine: decode the next block entry starting at "p",
// storing the number of shared key bytes, non_shared key bytes,
// and the length of the value in "*shared", "*non_shared", and
// "*value_length", respectively.  Will not dereference past "limit".
//
// If any errors are detected, returns nullptr.  Otherwise, returns a
// pointer to the key delta (just past the three decoded values).
static inline const char *DecodeEntry(const char *p, const char *limit,
                                      uint32_t *shared, uint32_t *non_shared,
                                      uint32_t *value_length) {
    if (limit - p < 3) return nullptr;
    *shared = reinterpret_cast<const unsigned char *>(p)[0];
    *non_shared = reinterpret_cast<const unsigned char *>(p)[1];
    *value_length = reinterpret_cast<const unsigned char *>(p)[2];
    if ((*shared | *non_shared | *value_length) < 128) {
        // Fast path: all three values are encoded in one byte each
        p += 3;
    } else {
        if ((p = GetVarint32Ptr(p, limit, shared)) == nullptr) return nullptr;
        if ((p = GetVarint32Ptr(p, limit, non_shared)) == nullptr) return nullptr;
        if ((p = GetVarint32Ptr(p, limit, value_length)) == nullptr) return nullptr;
    }

    if (static_cast<uint32_t>(limit - p) < (*non_shared + *value_length)) {
        return nullptr;
    }
    return p;
}

class Block::Iter : public Iterator {
private:
    friend class Table;

    const Comparator *const comparator_;
    const char *const data_;       // underlying block contents
    uint32_t const restarts_;      // Offset of restart array (list of fixed32)
    uint32_t const num_restarts_;  // Number of uint32_t entries in restart array

    // current_ is offset in data_ of current entry.  >= restarts_ if !Valid
    uint32_t current_;
    uint32_t restart_index_;  // Index of restart block in which current_ falls
    std::string key_;
    Slice value_;
    Status status_;

    inline int Compare(const Slice &a, const Slice &b) const {
        return comparator_->Compare(a, b);
    }

    // Return the offset in data_ just past the end of the current entry.
    inline uint32_t NextEntryOffset() const {
        return (value_.data() + value_.size()) - data_;
    }

    uint32_t GetRestartPoint(uint32_t index) {
        assert(index < num_restarts_);
        return DecodeFixed32(data_ + restarts_ + index * sizeof(uint32_t));
    }

    void SeekToRestartPoint(uint32_t index) {
        key_.clear();
        restart_index_ = index;
        // current_ will be fixed by ParseNextKey();

        // ParseNextKey() starts at the end of value_, so set value_ accordingly
        uint32_t offset = GetRestartPoint(index);
        value_ = Slice(data_ + offset, 0);
    }

public:
    Iter(const Comparator *comparator, const char *data, uint32_t restarts,
         uint32_t num_restarts)
            : comparator_(comparator),
              data_(data),
              restarts_(restarts),
              num_restarts_(num_restarts),
              current_(restarts_),
              restart_index_(num_restarts_) {
        assert(num_restarts_ > 0);
    }

    virtual bool Valid() const { return current_ < restarts_; }

    virtual Status status() const { return status_; }

    virtual Slice key() const {
        assert(Valid());
        return key_;
    }

    virtual Slice value() const {
        assert(Valid());
        return value_;
    }

    virtual void Next() {
        assert(Valid());
        ParseNextKey();
    }

    virtual void Prev() {
        assert(Valid());

        // Scan backwards to a restart point before current_
        const uint32_t original = current_;
        while (GetRestartPoint(restart_index_) >= original) {
            if (restart_index_ == 0) {
                // No more entries
                current_ = restarts_;
                restart_index_ = num_restarts_;
                return;
            }
            restart_index_--;
        }

        SeekToRestartPoint(restart_index_);
        do {
            // Loop until end of current entry hits the start of original entry
        } while (ParseNextKey() && NextEntryOffset() < original);
    }

    virtual void Seek(const Slice &target) {
        // Binary search in restart array to find the last restart point
        // with a key < target

        uint32_t left = 0;
        uint32_t right = num_restarts_ - 1;
        adgMod::Stats *instance = adgMod::Stats::GetInstance();
        while (left < right) {
            uint32_t mid = (left + right + 1) / 2;
            uint32_t region_offset = GetRestartPoint(mid);
            uint32_t shared, non_shared, value_length;
            const char *key_ptr =
                    DecodeEntry(data_ + region_offset, data_ + restarts_, &shared,
                                &non_shared, &value_length);
            if (key_ptr == nullptr || (shared != 0)) {
                CorruptionError();
                return;
            }
            Slice mid_key(key_ptr, non_shared);
            if (Compare(mid_key, target) < 0) {
                // Key at "mid" is smaller than "target".  Therefore all
                // blocks before "mid" are uninteresting.
                left = mid;
            } else {
                // Key at "mid" is >= "target".  Therefore all blocks at or
                // after "mid" are uninteresting.
                right = mid - 1;
            }
        }

        // Linear search (within restart block) for first key >= target
        SeekToRestartPoint(left);
        while (true) {
            if (!ParseNextKey() || Compare(key_, target) >= 0) {
                return;
            }
        }
    }

    virtual void SeekToFirst() {
        SeekToRestartPoint(0);
        ParseNextKey();
    }

    virtual void SeekToLast() {
        SeekToRestartPoint(num_restarts_ - 1);
        while (ParseNextKey() && NextEntryOffset() < restarts_) {
            // Keep skipping
        }
    }

private:
    inline void Seek(uint32_t left, uint32_t right, const Slice& target) {
//        if (right > num_restarts_ - 1)
//            right = num_restarts_ - 1;

        while (left < right) {
            uint32_t mid = (left + right) / 2;
            uint32_t region_offset = GetRestartPoint(mid);
            uint32_t shared, non_shared, value_length;
            const char *key_ptr =
                    DecodeEntry(data_ + region_offset, data_ + restarts_, &shared,
                                &non_shared, &value_length);
            if (key_ptr == nullptr || (shared != 0)) {
                CorruptionError();
                return;
            }
            Slice mid_key(key_ptr, non_shared);
            int comp = Compare(mid_key, target);
            if (comp < 0) {
                // Key at "mid" is smaller than "target".  Therefore all
                // blocks before "mid" are uninteresting.
                left = mid + 1;
            } else {
                // Key at "mid" is >= "target".  Therefore all blocks at or
                // after "mid" are uninteresting.
                right = mid;
            }
        }

        // Linear search (within restart block) for first key >= target
        SeekToRestartPoint(left);
        ParseNextKey();
//        while (true) {
//            if (!ParseNextKey() || Compare(key_, target) >= 0) {
//                return;
//            }
//        }
    }


    void CorruptionError() {
        current_ = restarts_;
        restart_index_ = num_restarts_;
        status_ = Status::Corruption("bad entry in block");
        key_.clear();
        value_.clear();
    }

    bool ParseNextKey() {
        current_ = NextEntryOffset();
        const char *p = data_ + current_;
        const char *limit = data_ + restarts_;  // Restarts come right after data
        if (p >= limit) {
            // No more entries to return.  Mark as invalid.
            current_ = restarts_;
            restart_index_ = num_restarts_;
            return false;
        }

        // Decode next entry
        uint32_t shared, non_shared, value_length;
        p = DecodeEntry(p, limit, &shared, &non_shared, &value_length);
        if (p == nullptr || key_.size() < shared) {
            CorruptionError();
            return false;
        } else {
            key_.resize(shared);
            key_.append(p, non_shared);
            value_ = Slice(p + non_shared, value_length);
            while (restart_index_ + 1 < num_restarts_ &&
                   GetRestartPoint(restart_index_ + 1) < current_) {
                ++restart_index_;
            }
            return true;
        }
    }
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_TABLE_BLOCK_H_
