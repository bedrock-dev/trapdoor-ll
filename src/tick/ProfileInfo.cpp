//
// Created by xhy on 2022/5/17.
//

#include "ProfileInfo.h"

#include <numeric>

#include "Msg.h"
#include "TrapdoorMod.h"

namespace tr {

    int64_t MSPTInfo::mean() const {
        return this->values.empty()
                   ? 0
                   : std::accumulate(values.begin(), values.end(), 0ll) /
                         static_cast<int64_t>(values.size());
    }

    void MSPTInfo::push(int64_t value) {
        this->values.push_back(value);
        if (this->values.size() > 20) {
            this->values.pop_front();
        }
    }

    int64_t MSPTInfo::min() const {
        if (values.empty()) {
            return 0;
        }
        auto min = values[0];
        for (auto v : values) {
            if (min > v) min = v;
        }
        return min;
    }

    int64_t MSPTInfo::max() const {
        if (values.empty()) {
            return 0;
        }
        auto max = values[0];
        for (auto v : values) {
            if (max < v) max = v;
        }
        return max;
    }

    double micro_to_mill(uint64_t v) { return static_cast<double>(v) / 1000.0; }

    void SimpleProfiler::Reset(SimpleProfiler::Type type) {
        this->type = type;
        this->chunk_info.reset();
    }

    void SimpleProfiler::Start(size_t round, SimpleProfiler::Type type) {
        this->Reset(type);
        this->profiling = true;
        this->current_round = 0;
        this->total_round = round;
    }

    void SimpleProfiler::Stop() {
        logger().info(fmt::format(
            "mspt: {}",
            tr::micro_to_mill(this->server_level_tick_time / total_round)));
        this->profiling = false;
        this->Reset(Normal);
    }

    void SimpleProfiler::Print() {
        switch (this->type) {
            case SimpleProfiler::Normal:
                this->PrintBasics();
                break;
            case SimpleProfiler::Actor:
                this->PrintActor();
                break;
            case SimpleProfiler::PendingTick:
                this->PrintPendingTicks();
                break;
            case SimpleProfiler::Chunk:
                this->PrintChunks();
                break;
        }
    }

    void SimpleProfiler::PrintChunks() const {}
    void SimpleProfiler::PrintPendingTicks() const {}
    void SimpleProfiler::PrintBasics() const {
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

        const double divide = 1000.0 * static_cast<double>(total_round);
        auto cf = [&](microsecond_t time) { return time / divide; };
        auto mspt = cf(server_level_tick_time);
        int tps = mspt <= 50 ? 20 : static_cast<int>(1000.0 / mspt);
        auto res = fmt::format(
            "- MSPT: {} TPS: {} Chunks\n"
            "- Redstone: {}\n"
            "  - Signal: {}\n"
            "  - Add: {}\n"
            "  - Update: {}\n"
            "  - Remove: {}\n"
            "- EntitySystems: {}\n"
            "- Chunk (un)load & village: {}\n"
            "- Chunk tick: {}\n"
            "  - BlockEntities: {}\n"
            "  - RandomTick: {}\n"
            "  - PendingTick: {}\n",
            mspt, tps, this->chunk_info.chunk_counter.size() / total_round,
            cf(redstone_info.sum()), cf(redstone_info.signal_update),
            cf(redstone_info.pending_add), cf(redstone_info.pending_update),
            cf(redstone_info.pending_remove), cf(entity_system_tick_time),
            cf(dimension_tick_time), 0, 0, 0, 0);
        tr::BroadcastMessage(res);
    }

    void SimpleProfiler::PrintActor() const {}
}  // namespace tr
