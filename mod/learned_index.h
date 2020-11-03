//
// Created by daiyi on 2020/02/02.
//

#ifndef LEVELDB_LEARNED_INDEX_H
#define LEVELDB_LEARNED_INDEX_H


#include <vector>
#include <cstring>
#include "util.h"
#include <atomic>
#include "plr.h"



using std::string;
using leveldb::Slice;
using leveldb::Version;
using leveldb::FileMetaData;



namespace adgMod {

    class LearnedIndexData;

    // An array collecting the total number of keys in a level in or before each file. One per level.
    // Used to get the target file when a level model produces the predicted position in the level. 
    class AccumulatedNumEntriesArray {
        friend class LearnedIndexData;

    public:
        std::vector<std::pair<uint64_t, string>> array;
    public:
        AccumulatedNumEntriesArray() = default;
        // During learning, add info to this array with the number of entries in a file and its largest key
        void Add(uint64_t num_entries, string&& key);
        // Given a predicted interval, return the target file index in param:index.
        bool Search(const Slice& key, uint64_t lower, uint64_t upper, size_t* index, uint64_t* relative_lower, uint64_t* relative_upper);
        // Used for testing assuming the model has no error
        bool SearchNoError(uint64_t position, size_t* index, uint64_t* relative_position);
        uint64_t NumEntries() const;
    };


    class VersionAndSelf {
    public:
        Version* version;
        int v_count;
        LearnedIndexData* self;
        int level;
    };

    class MetaAndSelf {
    public:
        Version* version;
        int v_count;
        FileMetaData* meta;
        LearnedIndexData* self;
        int level;
    };

    // The structure for learned index. Could be a file model or a level model
    class LearnedIndexData {
        friend class leveldb::Version;
        friend class leveldb::VersionSet;
    private:
        // predefined model error
        double error;
        // some flags used in online learning to control the state of the model
        std::atomic<bool> learned;
        std::atomic<bool> aborted;
        bool learned_not_atomic;
        std::atomic<bool> learning;
        // some params for level triggering policy, deprecated
        int allowed_seek;
        int current_seek;
    public:
        // is the data of this model filled (ready for learning)
        bool filled;
        // is this a level model
        bool is_level;

        // Learned linear segments and some other data needed
        std::vector<Segment> string_segments;
        uint64_t min_key;
        uint64_t max_key;
        uint64_t size;



    public:
        // all keys in the file/level to be leraned from
        std::vector<std::string> string_keys;
        // only used in level models
        AccumulatedNumEntriesArray num_entries_accumulated;

        int level;
        mutable int served;
        uint64_t cost;

//        int num_neg_model = 0, num_pos_model = 0, num_neg_baseline = 0, num_pos_baseline = 0;
//        uint64_t time_neg_model = 0, time_pos_model = 0, time_neg_baseline = 0, time_pos_baseline = 0;
//
//        int num_neg_model_p = 0, num_pos_model_p = 0, num_neg_baseline_p = 0, num_pos_baseline_p = 0, num_files_p = 0;
//        uint64_t time_neg_model_p = 0, time_pos_model_p = 0, time_neg_baseline_p = 0, time_pos_baseline_p = 0;
//        double gain_p = 0;
//        uint64_t file_size = 0;




        explicit LearnedIndexData(int allowed_seek, bool level_model) : error(level_model?level_model_error:file_model_error), learned(false), aborted(false), learning(false),
            learned_not_atomic(false), allowed_seek(allowed_seek), current_seek(0), filled(false), is_level(level_model), level(0), served(0), cost(0) {};
        LearnedIndexData(const LearnedIndexData& other) = delete;

        // Inference function. Return the predicted interval.
        // If the key is in the training set, the output interval guarantees to include the key
        // otherwise, the output is undefined!
        // If the output lower bound is larger than MaxPosition(), the target key is not in the file
        std::pair<uint64_t, uint64_t> GetPosition(const Slice& key) const;
        uint64_t MaxPosition() const;
        double GetError() const;
        
        // Learning function and checker (check if this model is available)
        bool Learn();
        bool Learned();
        bool Learned(Version* version, int v_count, int level);
        bool Learned(Version* version, int v_count, FileMetaData* meta, int level);
        static void LevelLearn(void* arg, bool no_lock=false);
        static uint64_t FileLearn(void* arg);

        // Load all the keys in the file/level
        bool FillData(Version* version, FileMetaData* meta);

        // writing this model to disk and load this model from disk
        void WriteModel(const string& filename);
        void ReadModel(const string& filename);
        
        // print model stats
        void ReportStats();

        // test functions when developing CBA...
        void FillCBAStat(bool positive, bool model, uint64_t time);

        bool Learn(bool file);
    };

    // an array storing all file models and provide similar access interface with multithread protection
    class FileLearnedIndexData {
    private:
        leveldb::port::Mutex mutex;
        std::vector<LearnedIndexData*> file_learned_index_data;
    public:
        uint64_t watermark;


        bool Learned(Version* version, FileMetaData* meta, int level);
        bool FillData(Version* version, FileMetaData* meta);
        std::vector<std::string>& GetData(FileMetaData* meta);
        std::pair<uint64_t, uint64_t> GetPosition(const Slice& key, int file_num);
        AccumulatedNumEntriesArray* GetAccumulatedArray(int file_num);
        LearnedIndexData* GetModel(int number);
        void Report();
        ~FileLearnedIndexData();
    };

    class LevelLearnedIndexData {
     private:
      leveldb::port::Mutex mutex;
      std::vector<LearnedIndexData*> level_learned_index_data;
     public:

    };


}

#endif //LEVELDB_LEARNED_INDEX_H
