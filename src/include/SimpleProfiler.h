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
#define TIMER_END                              \
    auto elapsed = timer_clock::now() - start; \
    long long timeResult = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

namespace trapdoor {
    using std::chrono::microseconds;

    double micro_to_mill(uint64_t v);

    struct MSPTInfo {
        std::deque<int64_t> values;

        int64_t mean() const;

        void push(int64_t value);

        int64_t min() const;

        int64_t max() const;

        std::pair<int64_t, int64_t> pairs() const;
    };

    struct ChunkProfileInfo {
        std::array<std::map<trapdoor::TBlockPos2, std::vector<microsecond_t>>, 3> chunk_counter{};
        microsecond_t blockEntitiesTickTime = 0;
        microsecond_t randomTickTime = 0;
        microsecond_t pendingTickTime = 0;
        size_t totalTickTime = 0;

        inline size_t getChunkNumber() const {
            size_t num = 0;
            for (auto &m : chunk_counter) {
                num += m.size();
            }
            return num;
        }

        inline void reset() {
            for (int i = 0; i < 3; i++) {
                this->chunk_counter[i].clear();
            }

            blockEntitiesTickTime = 0;
            randomTickTime = 0;
            pendingTickTime = 0;
            totalTickTime = 0;
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
        microsecond_t signalUpdate = 0;   // Dimension::tickRedstone
        microsecond_t pendingAdd = 0;     // PendingAdd
        microsecond_t pendingUpdate = 0;  // PendingUpdate
        microsecond_t pendingRemove = 0;  // pendingRemove
        inline void reset() {
            signalUpdate = 0;
            pendingAdd = 0;
            pendingUpdate = 0;
            pendingRemove = 0;
        }

        inline microsecond_t sum() const {
            return signalUpdate + pendingAdd + pendingRemove + pendingUpdate;
        }
    };

    // 普通profile

    struct EntityInfo {
        microsecond_t time;
        int count;
    };
    struct SimpleProfiler {
        enum Type { Normal, Chunk, PendingTick, Entity };
        SimpleProfiler::Type type = Normal;
        bool profiling = false;
        size_t totalRound = 100;
        size_t currentRound = 0;
        ChunkProfileInfo chunkInfo{};
        std::array<std::map<std::string, EntityInfo>, 3> actorInfo{};
        RedstoneProfileInfo redstoneInfo{};
        microsecond_t serverLevelTickTime = 0;  // mspt
        microsecond_t dimensionTickTime = 0;    // 区块加载卸载&村庄
        microsecond_t entitySystemTickTime = 0;

        void print() const;

        void printChunks() const;

        void printPendingTicks() const;

        void printBasics() const;

        void printActor() const;

        void reset(SimpleProfiler::Type type);

        void start(size_t round, SimpleProfiler::Type type = Normal);
        void stop();
    };

}  // namespace trapdoor

#endif  // TRAPDOOR_SIMPLE_PROFILER_H
