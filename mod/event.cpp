//
// Created by daiyi on 2020/04/25.
//

#include <iostream>
#include "event.h"

using std::cout;

void CompactionEvent::Report() {
    printf("CompactionEvent %s %lu %lu\n", level.c_str(), time.first, time.second);
}

void LearnEvent::Report() {
    printf("LearnEvent %d %d %d %lu %lu\n", type, level, succeeded, time.first, time.second);
}

void WorkloadEvent::Report() {
    printf("WorkloadEvent %d %d %d %d %d %d %d %lu %lu\n", level_get, file_get, baseline_get, success_get, false_get,
                    num_compaction, num_learn, read_time, write_time);
    for (uint64_t time : detailed_times) {
        printf("%lu ", time);
    }
    printf("\n");
}