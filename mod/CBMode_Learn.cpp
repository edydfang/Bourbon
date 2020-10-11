#include <util/mutexlock.h>
#include <iostream>
#include "CBMode_Learn.h"
#include "learned_index.h"


CBModel_Learn::CBModel_Learn() : negative_lookups_time(2), positive_lookups_time(2) {};

void CBModel_Learn::AddLookupData(int level, bool positive, bool model, uint64_t value) {
    leveldb::MutexLock guard(&lookup_mutex);
    std::vector<Counter>& target = positive ? positive_lookups_time : negative_lookups_time;
    target[model].Increment(level, value);
}

void CBModel_Learn::AddFileData(int level, uint64_t num_negative, uint64_t num_positive, uint64_t size) {
    leveldb::MutexLock guard(&file_mutex);
    num_negative_lookups_file.Increment(level, num_negative);
    num_positive_lookups_file.Increment(level, num_positive);
    file_sizes.Increment(level, size);
}

void CBModel_Learn::AddLearnCost(int level, uint64_t cost, uint64_t size) {
    learn_costs.Increment(level, cost);
    learn_sizes.Increment(level, size);
}

double CBModel_Learn::CalculateCB(int level, uint64_t file_size) {
    // used for simple testing different learning policies, not used now
    if (adgMod::policy == 2) return 0;

    int num_pos[2] = {0, 0}, num_neg[2] = {0, 0}, num_files = 0, num_learn = 0;
    uint64_t time_pos[2] = {0, 0}, time_neg[2] = {0, 0}, num_neg_lookups_file, num_pos_lookups_file, size_sum, cost_sum, learn_size_sum;
    {
        leveldb::MutexLock guard(&lookup_mutex);
        for (int i = 0; i < 2; ++i) {
            num_pos[i] = positive_lookups_time[i].nums[level];
            num_neg[i] = negative_lookups_time[i].nums[level];
            time_pos[i] = positive_lookups_time[i].counts[level];
            time_neg[i] = negative_lookups_time[i].counts[level];
        }
    }
    {
        leveldb::MutexLock guard(&file_mutex);
        num_files = num_negative_lookups_file.nums[level];
        num_neg_lookups_file = num_negative_lookups_file.counts[level];
        num_pos_lookups_file = num_positive_lookups_file.counts[level];
        size_sum = file_sizes.counts[level];
    }
//    num_learn = learn_costs.nums[level];
//    cost_sum = learn_costs.counts[level];
//    learn_size_sum = learn_sizes.counts[level];

    if (num_files < file_average_limit[level]) return const_size_to_cost + 1;
    double average_pos_lookups = (double) num_pos_lookups_file / num_files;
    double average_neg_lookups = (double) num_neg_lookups_file / num_files;
    double average_pos_time[2] = {0, 0}, average_neg_time[2] = {0, 0};

    for (int i = 0; i < 2; ++i) {
        if (num_pos[i] + num_neg[i] < lookup_average_limit) {
            return 0;
        }

        if (num_pos[i] < 500) {
            average_pos_lookups = 0;
            average_pos_time[i] = 0;
        } else {
            average_pos_time[i] = (double) time_pos[i] / num_pos[i];
        }

        if (num_neg[i] < 500) {
            average_neg_lookups = 0;
            average_neg_time[i] = 0;
        } else {
            average_neg_time[i] = (double) time_neg[i] / num_neg[i];
        }
    }

    double pos_gain = (average_pos_time[0] - average_pos_time[1]) * average_pos_lookups;
    double neg_gain = (average_neg_time[0] - average_neg_time[1]) * average_neg_lookups;

//    model->num_pos_baseline_p = num_pos[0];
//    model->num_pos_model_p = num_pos[1];
//    model->num_neg_baseline_p = num_neg[0];
//    model->num_neg_model_p = num_neg[1];
//    model->time_pos_baseline_p = time_pos[0];
//    model->time_pos_model_p = time_pos[1];
//    model->time_neg_baseline_p = time_neg[0];
//    model->time_neg_model_p = time_neg[1];
//    model->file_size = file_size;
//    model->gain_p = pos_gain + neg_gain;
//    //std::cout << model->gain_p << std::endl;
//    model->num_files_p = num_files;

    // used for simple testing different learning policies, not used now
    if (adgMod::policy == 1) return const_size_to_cost + 1;
    if (adgMod::policy == 2) return 0;

//    fprintf(stdout, "%f %lu %d %f %f %f %f %f %f\n", pos_gain + neg_gain, file_size * const_size_to_cost, level,
//             average_pos_time[0], average_pos_time[1], average_pos_lookups, average_neg_time[0], average_neg_time[1], average_neg_lookups);
    return (pos_gain + neg_gain) / size_sum * num_files;//(num_learn >= 20 ? (double) cost_sum / learn_size_sum : const_size_to_cost);
}



void CBModel_Learn::Report() {
    negative_lookups_time[0].name = "BaselineNegative";
    negative_lookups_time[1].name = "LLSMNegative";
    positive_lookups_time[0].name = "BaselinePositive";
    positive_lookups_time[1].name = "LLSMPositive";

    negative_lookups_time[0].Report();
    negative_lookups_time[1].Report();
    positive_lookups_time[0].Report();
    positive_lookups_time[1].Report();
}
























