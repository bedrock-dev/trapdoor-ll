//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_SIMPLE_PROFILER_H
#define TRAPDOOR_SIMPLE_PROFILER_H

#include <array>
#include <chrono>
#include <deque>
#include <map>

#include "TBlockPos.h"

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
        std::array<std::map<tr::TBlockPos2, microsecond_t>, 3> chunk_counter{};
        microsecond_t block_entities_tick_time = 0;
        microsecond_t random_tick_time = 0;
        microsecond_t pending_tick_time = 0;
        size_t total_tick_time = 0;

        inline void reset() {
            for (int i = 0; i < 3; i++) {
                this->chunk_counter[i].clear();
            }

            block_entities_tick_time = 0;
            random_tick_time = 0;
            pending_tick_time = 0;
            total_tick_time = 0;
        }
    };

    /*
    ServerLevel::tick
     - Redstone
        - Dimension::tickRedstone(shouldUpdate,cacueValue,evaluate)
        - pendingUpdate
        - pendinnRemove
        - pendingAdd
     - Dimension::tick(chunk load/village)
     - entitySystem
     - Lvevl::tick
        - LevelChunk::Tick
            - blockEnties
            - randomChunk
            - Actor::tick(non global)
    */

    struct RedstoneProfileInfo {
        microsecond_t signal_update = 0;   // Dimension::tickRedstone
        microsecond_t pending_add = 0;     // PendingAdd
        microsecond_t pending_update = 0;  // PendingUpdate
        microsecond_t pending_remove = 0;  // pendingRemove
        inline void reset() {
            signal_update = 0;
            pending_add = 0;
            pending_update = 0;
            pending_remove = 0;
        }

        inline microsecond_t sum() const {
            return signal_update + pending_add + pending_remove +
                   pending_update;
        }
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

    //普通profile
    struct SimpleProfiler {
        enum Type { Normal, Chunk, PendingTick, Actor };

        SimpleProfiler::Type type = Normal;
        bool profiling = false;
        size_t total_round = 100;
        size_t current_round = 0;
        ChunkProfileInfo chunk_info{};
        RedstoneProfileInfo redstone_info{};

        microsecond_t server_level_tick_time = 0;  // mspt
        microsecond_t dimension_tick_time = 0;     //区块加载卸载&村庄
        microsecond_t entity_system_tick_time = 0;

        void Print();

        void PrintChunks() const;

        void PrintPendingTicks() const;

        void PrintBasics() const;

        void PrintActor() const;

        void Reset(SimpleProfiler::Type type);
        void Start(size_t round, SimpleProfiler::Type type = Normal);
        void Stop();
    };

}  // namespace tr

#endif  // TRAPDOOR_PROFILEINFO_H
