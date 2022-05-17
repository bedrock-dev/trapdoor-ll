//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_PROFILEINFO_H
#define TRAPDOOR_PROFILEINFO_H

#include <deque>

#include <chrono>

typedef std::chrono::high_resolution_clock timer_clock;
#define TIMER_START auto start = timer_clock::now();
#define TIMER_END                                                      \
    auto elapsed = timer_clock::now() - start;                         \
    long long timeReslut =                                             \
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed) \
            .count();

namespace tr {


    double micro_to_mill(uint64_t v);

    struct MSPTInfo {

        std::deque<int64_t> values;

        int64_t mean() const;

        void push(int64_t value);

        int64_t min() const;

        int64_t max() const;

    };
}


#endif //TRAPDOOR_PROFILEINFO_H
