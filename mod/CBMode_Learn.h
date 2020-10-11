// CBA

#ifndef LEVELDB_CBMODE_LEARN_H
#define LEVELDB_CBMODE_LEARN_H


#include "Counter.h"
#include <vector>
#include <queue>

static const int file_average_limit[7] = {10, 20, 20, 20, 20, 500, 500};

namespace adgMod {
    class LearnedIndexData;
}


class CBModel_Learn {
private:
    std::vector<Counter> negative_lookups_time;
    std::vector<Counter> positive_lookups_time;

//    std::queue<int> num_positive_lookups_file;
//    std::queue<int> num_negative_lookups_file;

    Counter num_positive_lookups_file;
    Counter num_negative_lookups_file;
    Counter file_sizes;

    Counter learn_costs;
    Counter learn_sizes;

    leveldb::port::Mutex lookup_mutex;
    leveldb::port::Mutex file_mutex;
public:
    static const int const_size_to_cost = 10;
    static const int lookup_average_limit = 10000;

    CBModel_Learn();
    // functions that record data during runtime
    void AddLookupData(int level, bool positive, bool model, uint64_t value);
    void AddFileData(int level, uint64_t num_negative, uint64_t num_positive, uint64_t size);
    void AddLearnCost(int level, uint64_t cost, uint64_t size);
    
    // check if a model is benefitial to learn
    double CalculateCB(int level, uint64_t file_size);
    // report collected stats
    void Report();
};

#endif //LEVELDB_CBMODE_LEARN_H
