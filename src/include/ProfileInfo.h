//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_PROFILEINFO_H
#define TRAPDOOR_PROFILEINFO_H

#include <chrono>
#include <deque>
#include <map>

#include "BlockPos.h"

typedef std::chrono::high_resolution_clock timer_clock;
typedef int64_t microsecond_t;
#define TIMER_START auto start = timer_clock::now();
#define TIMER_END                                                      \
    auto elapsed = timer_clock::now() - start;                         \
    long long timeResult =                                             \
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed) \
            .count();

namespace tr {

    using std::chrono::microseconds;

    double micro_to_mill(uint64_t v);

    struct MSPTInfo {
        std::deque<int64_t> values;

        int64_t mean() const;

        void push(int64_t value);

        int64_t min() const;

        int64_t max() const;
    };

    struct ChunkProfileInfo {
        std::map<ChunkPos, microsecond_t> chunk_counter{};
        microsecond_t block_entities_tick_time = 0;
        microsecond_t random_tick_time = 0;
        microsecond_t pending_tick_time = 0;
        size_t total_tick_time = 0;

        void reset() {
            chunk_counter.clear();
            block_entities_tick_time = 0;
            random_tick_time = 0;
            pending_tick_time = 0;
            total_tick_time = 0;
        }
    };

    //普通profile
    struct NormalProfiler {
        bool profiling = false;
        ChunkProfileInfo chunk_info{};
        size_t total_round = 100;
        size_t current_round = 0;
        microsecond_t server_level_tick_time = 0;  // mspt
        microsecond_t dimension_tick_time = 0;     //区块加载卸载&村庄
        microsecond_t entity_system_tick_time = 0;

        void Reset();
        void Start(size_t round);
        void Stop();
    };

    //实体profile
    struct ActorProfiler {
        struct ActorProfilerInfo {
            microsecond_t time = 0;
            size_t count = 0;
        };
        bool profiling = false;
        size_t total_round = 100;
        size_t current_round = 0;
        std::map<std::string, ActorProfilerInfo> actor_ticking_list;
    };

    //    //红石统计数据
    //    struct RedstoneStat {
    //        microsecond_t signals = 0;
    //        microsecond_t pendingAdd = 0;
    //        microsecond_t pendingRemove = 0;
    //        microsecond_t pendingUpdate = 0;
    //
    //        inline void reset() {
    //            signals = 0;
    //            pendingAdd = 0;
    //            pendingRemove = 0;
    //            pendingUpdate = 0;
    //        }
    //
    //        microsecond_t total() const {
    //            return signals + pendingAdd + pendingRemove + pendingUpdate;
    //        }
    //    };
    //
    //    class SimpleProfiler : {
    //    public:
    //
    //        ChunkStat chunkStat;
    //        RedstoneStat redstonStat;
    //        microsecond_t serverLevelTickTime = 0;  // mspt
    //        microsecond_t dimensionTickTime = 0;    //区块加载卸载&村庄
    //        microsecond_t levelEntitySystemTickTime = 0;
    //        std::map<trapdoor::BlockPos2, size_t> ptCounter;
    //        size_t totalRound = 100;
    //        size_t currentRound = 0;
    //        bool inProfiling = false;
    //
    //        //重重所有数据
    //        void reset() {
    //            serverLevelTickTime = 0;  // mspt
    //            dimensionTickTime = 0;    //区块加载卸载&村庄
    //            chunkStat.reset();
    //            levelEntitySystemTickTime = 0;  //实体系统更新
    //            redstonStat.reset();
    //            currentRound = 0;
    //            this->ptCounter.clear();
    //        }

}  // namespace tr

#endif  // TRAPDOOR_PROFILEINFO_H
