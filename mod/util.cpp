//
// Created by daiyi on 2020/02/02.
//

#include <util/mutexlock.h>
#include "util.h"
#include "learned_index.h"
#include <x86intrin.h>

using std::to_string;

namespace adgMod {

    int MOD = 5;
    bool string_mode = true;
    uint64_t key_multiple = 1;
    uint32_t file_model_error = 10;
    uint32_t level_model_error = 1;
    int block_restart_interval = 16;
    uint32_t test_num_level_segments = 100;
    uint32_t test_num_file_segments = 100;
    int key_size;
    int value_size;
    leveldb::Env* env;
    leveldb::DBImpl* db;
    leveldb::ReadOptions read_options;
    leveldb::WriteOptions write_options;
    FileLearnedIndexData* file_data = nullptr;
    CBModel_Learn* learn_cb_model = nullptr;
    uint64_t fd_limit = 1024 * 1024;
    bool use_filter = false;
    bool restart_read = false;
    bool fresh_write = false;
    bool reopen = false;

    // the time we wait before learning (as the file may die within this short time and
    // if we learn, we waste the learning)
    uint64_t learn_trigger_time = 50000000;
    int policy = 0;
    std::atomic<int> num_read(0);
    std::atomic<int> num_write(0);

    int file_allowed_seek = 10;
    int level_allowed_seek = 1;
    float reference_frequency = 2.6;
    bool block_num_entries_recorded = false;
    bool level_learning_enabled = false;
    bool file_learning_enabled = true;
    bool load_level_model = true;
    bool load_file_model = true;
    uint64_t block_num_entries = 0;
    uint64_t block_size = 0;
    uint64_t entry_size = 0;


    vector<Counter> levelled_counters(15);
    vector<vector<Event*>> events(3);
    leveldb::port::Mutex compaction_counter_mutex;
    leveldb::port::Mutex learn_counter_mutex;
    leveldb::port::Mutex file_stats_mutex;
    map<int, FileStats> file_stats;

    uint64_t ExtractInteger(const char* pos, size_t size) {
        char* temp = new char[size + 1];
        memcpy(temp, pos, size);
        temp[size] = '\0';
        uint64_t result = (uint64_t) atol(temp);
        delete[] temp;
        return result;
    }

//    bool SearchNumEntriesArray(const std::vector<uint64_t>& num_entries_array, const uint64_t pos,
//                                size_t* index, uint64_t* relative_pos) {
//        size_t left = 0, right = num_entries_array.size() - 1;
//        while (left < right) {
//            size_t mid = (left + right) / 2;
//            if (pos < num_entries_array[mid]) right = mid;
//            else left = mid + 1;
//        }
//        *index = left;
//        *relative_pos = left > 0 ? pos - num_entries_array[left - 1] : pos;
//        return left < num_entries_array.size();
//    }


    string generate_key(const string& key) {
        string result = string(key_size - key.length(), '0') + key;
        return std::move(result);
    }

    string generate_value(uint64_t value) {
        string value_string = to_string(value);
        string result = string(value_size - value_string.length(), '0') + value_string;
        return std::move(result);
    }

    uint64_t SliceToInteger(const Slice& slice) {
        const char* data = slice.data();
        size_t size = slice.size();
        uint64_t num = 0;
        bool leading_zeros = true;

        for (int i = 0; i < size; ++i) {
            int temp = data[i];
            if (leading_zeros && temp == '0') continue;
            leading_zeros = false;
            num = (num << 3) + (num << 1) + temp - 48;
        }
        return num;
    }

    int compare(const Slice& slice, const string& string) {
        return memcmp((void*) slice.data(), string.c_str(), slice.size());
    }

    bool operator<(const Slice& slice, const string& string) {
        return memcmp((void*) slice.data(), string.c_str(), slice.size()) < 0;
    }
    bool operator>(const Slice& slice, const string& string) {
        return memcmp((void*) slice.data(), string.c_str(), slice.size()) > 0;
    }
    bool operator<=(const Slice& slice, const string& string) {
        return memcmp((void*) slice.data(), string.c_str(), slice.size()) <= 0;
    }
    bool operator>=(const Slice& slice, const string& string) {
        return memcmp((void*) slice.data(), string.c_str(), slice.size()) >= 0;
    }

    uint64_t get_time_difference(timespec start, timespec stop) {
        return (stop.tv_sec - start.tv_sec) * 1000000000 + stop.tv_nsec - start.tv_nsec;
    }

}