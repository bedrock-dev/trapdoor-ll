#ifndef TRAPDOOR_GAME_TICK_H
#define TRAPDOOR_GAME_TICK_H
#include <mc/Block.hpp>
#include <mc/BlockPos.hpp>

#include "SimpleProfiler.h"
namespace trapdoor {
    struct ActionResult;

    // Data structures for Pending Tick
    struct TTickNextTickData {
        BlockPos pos;
        Block* block{nullptr};
        uint64_t tick{0};
        int priorityOffset{};
    };

    struct TBlockTick {
        bool removed{};
        TTickNextTickData data;
    };
    struct TTickDataSet {
        std::vector<TBlockTick> queue;
    };
    struct BlockTickingQueue {
        void* owningChunk{nullptr};
        uint64_t currentTick{0};
        TTickDataSet next;
        TTickDataSet active;
        bool queueType{};
        bool installTick{};
    };

    enum class TickingStatus { Normal, Forwarding, SlowDown, Frozen, Acc, Warp };

    struct TickingInfo {
        int remainWarpTick = 0;
        size_t slowDownTime = 1;
        size_t forwardTickNum = 0;
        size_t accTime = 1;
        size_t slowDownCounter = 0;
        TickingStatus status = TickingStatus::Normal;
        TickingStatus oldStatus = TickingStatus::Normal;
    };

    double getMeanMSPT();

    double getMeanTPS();

    // Tick and prof Command action
    ActionResult printMSPT();
    ActionResult freezeWorld();

    ActionResult resetWorld();

    ActionResult forwardWorld(int gt);

    ActionResult slowDownWorld(int times);

    ActionResult accWorld(int times);

    ActionResult warpWorld(int times);

    ActionResult queryWorld();

    ActionResult cancelWorld();

    ActionResult startProfiler(int rounds, SimpleProfiler::Type type);

}  // namespace trapdoor
#endif
