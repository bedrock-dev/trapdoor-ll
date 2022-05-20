//
// Created by xhy on 2022/5/17.
//

#include "SimpleProfiler.h"

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
        this->redstone_info.reset();
        this->chunk_info.reset();
        this->server_level_tick_time = 0;
        this->dimension_tick_time = 0;
        this->entity_system_tick_time = 0;
    }

    void SimpleProfiler::Start(size_t round, SimpleProfiler::Type type) {
        tr::logger().debug("Begin profiling with total round {}", round);
        this->Reset(type);
        this->profiling = true;
        this->current_round = 0;
        this->total_round = round;
    }

    void SimpleProfiler::Stop() {
        tr::logger().debug("Stop profiling");
        this->profiling = false;
        this->Print();
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
        tr::logger().debug("divide = {}", divide);
        auto cf = [divide](microsecond_t time) { return time * 1.0f / divide; };
        auto mspt = cf(server_level_tick_time);
        int tps = mspt <= 50 ? 20 : static_cast<int>(1000.0 / mspt);
        auto res = fmt::format(
            "- MSPT: {:.3f} ms TPS: {} Chunks: {}\n"
            "- Redstone: {:.3f} ms\n"
            "  - Signal: {:.3f} ms\n"
            "  - Add: {:.3f} ms\n"
            "  - Update: {:.3f} ms\n"
            "  - Remove: {:.3f} ms\n"
            "- EntitySystems: {:.3f} ms\n"
            "- Chunk (un)load & village: {:.3f} ms\n"
            "- Chunk tick: {:.3f} ms\n"
            "  - BlockEntities: {:.3f} ms\n"
            "  - RandomTick: {:.3f} ms\n"
            "  - PendingTick: {:.3f} ms\n",

            /*summary*/
            mspt, tps, this->chunk_info.chunk_counter.size() / total_round,
            /*redstone*/
            cf(redstone_info.sum()), cf(redstone_info.signal_update),  //
            cf(redstone_info.pending_add), cf(redstone_info.pending_update),
            cf(redstone_info.pending_remove),  //
            /*entities system & dimension*/
            cf(entity_system_tick_time), cf(dimension_tick_time),  //
            /*chunks*/
            cf(chunk_info.total_tick_time),
            cf(chunk_info.block_entities_tick_time),
            cf(chunk_info.random_tick_time), cf(chunk_info.pending_tick_time));
        tr::BroadcastMessage(res);
    }

    void SimpleProfiler::PrintActor() const {}
}  // namespace tr
