//
// Created by daiyi on 2020/04/25.
//

#ifndef LEVELDB_EVENT_H
#define LEVELDB_EVENT_H

#include <string>
#include <utility>
#include <cstdint>
#include <vector>



using std::string;



class Event {
protected:
    std::pair<uint64_t, uint64_t> time;

public:
    explicit Event(std::pair<uint64_t, uint64_t> time_) : time(time_) {};
    virtual void Report() = 0;
    virtual ~Event() = default;
};

class CompactionEvent : public Event {
private:
    string level;
public:
    CompactionEvent(std::pair<uint64_t, uint64_t> time_, string&& level_) : Event(time_), level(level_) {};
    void Report();
};

class LearnEvent : public Event {
private:
    int type;
    int level;
    bool succeeded;
public:
    LearnEvent(std::pair<uint64_t, uint64_t> time_, int type_, int level_, int succeeded_) : Event(time_), type(type_), level(level_), succeeded(succeeded_) {};
    void Report();
};

class WorkloadEvent : public Event {
private:
    int level_get;
    int file_get;
    int baseline_get;
    int success_get;
    int false_get;
    int num_compaction;
    int num_learn;
    uint64_t read_time;
    uint64_t write_time;
    std::vector<uint64_t> detailed_times;
public:
    WorkloadEvent(std::pair<uint64_t, uint64_t> time_, int level, int file, int baseline, int success, int failed,
                    int compaction, int learn, uint64_t read, uint64_t write, std::vector<uint64_t>&& times) :
                    Event(time_), level_get(level), file_get(file), baseline_get(baseline), success_get(success), false_get(failed),
                    num_compaction(compaction), num_learn(learn), read_time(read), write_time(write), detailed_times(times) {};
    void Report() override;
};

#endif //LEVELDB_EVENT_H
