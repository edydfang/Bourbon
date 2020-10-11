//
// Created by daiyi on 2020/02/02.
// a collection of global variables and flags/states/parameters

#ifndef LEVELDB_UTIL_H
#define LEVELDB_UTIL_H

#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "db/db_impl.h"
#include "leveldb/slice.h"
#include "leveldb/env.h"
#include "Counter.h"
#include "event.h"
#include "CBMode_Learn.h"
#include <x86intrin.h>


using std::string;
using std::vector;
using std::map;
using leveldb::Slice;


namespace adgMod {

    class FileLearnedIndexData;
    class LearnedIndexData;
    class FileStats;

    // I keep several versions during development to ease testing. Currently use 7&8
    // MOD=0 means original LevelDB
    // MOD=6 means Learned Index without WiscKey port
    // MOD=7 means Bourbon (Learned Index + WiscKey)
    // MOD=8 means WiscKey (simple port)
    extern int MOD;
    // used in early testing
    extern bool string_mode;
    extern uint64_t key_multiple;
    extern uint32_t model_error;
    extern int block_restart_interval;
    extern uint32_t test_num_level_segments;
    extern uint32_t test_num_file_segments;

    // some variables and pointers made global
    extern int key_size;
    extern int value_size;
    extern leveldb::Env* env;
    extern leveldb::DBImpl* db;
    extern leveldb::ReadOptions read_options;
    extern leveldb::WriteOptions write_options;

    // File model array
    extern FileLearnedIndexData* file_data;
    // Cost vs. Benefit Analyzer
    extern CBModel_Learn* learn_cb_model;
    // During our experiments, we unlimit file descriptor numbers. LevelDB default is 1024
    extern uint64_t fd_limit;
    extern bool use_filter;
    extern bool restart_read;
    extern bool fresh_write;
    extern bool reopen;
    extern uint64_t learn_trigger_time;
    extern int policy;
    extern std::atomic<int> num_read;
    extern std::atomic<int> num_write;

    extern int file_allowed_seek;
    extern int level_allowed_seek;
    extern float reference_frequency;
    extern bool block_num_entries_recorded;

    // enable online file learning -- default=false 
    extern bool level_learning_enabled;
    // enable online level learning -- default=true
    extern bool file_learning_enabled;
    // load offline-learned level models -- default=true
    extern bool load_level_model;
    // load offline-learned file models -- default=true
    extern bool load_file_model;
    
    // constants determined during the first offline learning following the load of DB
    extern uint64_t block_num_entries;
    extern uint64_t block_size;
    extern uint64_t entry_size;

    // runtime data collectors
    extern vector<Counter> levelled_counters;
    extern vector<vector<Event*>> events;
    extern leveldb::port::Mutex compaction_counter_mutex;
    extern leveldb::port::Mutex learn_counter_mutex;
    extern leveldb::port::Mutex file_stats_mutex;
    // data for CBA
    extern map<int, FileStats> file_stats;


    // some util functions
    uint64_t ExtractInteger(const char* pos, size_t size);
//bool SearchNumEntriesArray(const std::vector<uint64_t>& num_entries_array, const uint64_t position, size_t* index, uint64_t* relative_position);
    string generate_key(const string& key);
    string generate_value(uint64_t value);
    uint64_t SliceToInteger(const Slice& slice);
    int compare(const Slice& slice, const string& string);
    bool operator<(const Slice& slice, const string& string);
    bool operator>(const Slice& slice, const string& string);
    bool operator<=(const Slice& slice, const string& string);
    bool operator>=(const Slice& slice, const string& string);
    uint64_t get_time_difference(timespec start, timespec stop);

    // data structure containing infomation for CBA
    class FileStats {
    public:
        uint64_t start;
        uint64_t end;
        int level;
        uint32_t num_lookup_neg;
        uint32_t num_lookup_pos;
        uint64_t size;

        explicit FileStats(int level_, uint64_t size_) : start(0), end(0), level(level_), num_lookup_pos(0), num_lookup_neg(0), size(size_) {
            adgMod::Stats* instance = adgMod::Stats::GetInstance();
            uint32_t dummy;
            start = (__rdtscp(&dummy) - instance->initial_time) / adgMod::reference_frequency;
        };

        void Finish() {
            adgMod::Stats* instance = adgMod::Stats::GetInstance();
            uint32_t dummy;
            end = (__rdtscp(&dummy) - instance->initial_time) / adgMod::reference_frequency;
        }
    };
}


#endif //LEVELDB_UTIL_H
