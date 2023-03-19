//
// Created by xhy on 2022/5/17.
//
#include "SimpleProfiler.h"

#include <FMT/format.h>

#include <algorithm>
#include <mc/I18n.hpp>
#include <numeric>

#include "Msg.h"
#include "TrapdoorMod.h"
#include "Utils.h"

namespace trapdoor {
    int64_t MSPTInfo::mean() const {
        return this->values.empty() ? 0
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
    std::pair<int64_t, int64_t> MSPTInfo::pairs() const {
        int64_t v1 = 0, cv1 = 0;
        int64_t v2 = 0, cv2 = 0;
        for (int i = 0; i < values.size(); i++) {
            if (i % 2 == 0) {
                v1 += values[i];
                cv1++;
            } else {
                v2 += values[i];
                cv2++;
            }
        }
        if (cv1 != 0) v1 /= cv1;
        if (cv2 != 0) v2 /= cv2;
        if (v1 > v2) {
            std::swap(v1, v2);
        }
        return {v1, v2};
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

    void SimpleProfiler::reset(SimpleProfiler::Type t) {
        this->type = t;
        this->redstoneInfo.reset();
        this->chunkInfo.reset();
        this->gameSessionTickTime = 0;
        this->dimensionTickTime = 0;
        this->entitySystemTickTime = 0;
        for (auto &m : this->actorInfo) {
            m.clear();
        }

        for (auto &pt : this->ptCounter) {
            pt.clear();
        }
    }

    void SimpleProfiler::start(size_t round, SimpleProfiler::Type t) {
        trapdoor::logger().debug("Begin profiling with total round {}", round);
        this->reset(t);
        this->profiling = true;
        this->currentRound = 0;
        this->totalRound = round;
    }

    void SimpleProfiler::stop() {
        trapdoor::logger().debug("stop profiling");
        this->profiling = false;
        this->print();
        this->reset(Normal);
    }

    void SimpleProfiler::print() const {
        switch (this->type) {
            case SimpleProfiler::Normal:
                this->printBasics();
                break;
            case SimpleProfiler::Entity:
                this->printActor();
                break;
            case SimpleProfiler::PendingTick:
                this->printPendingTicks();
                break;
            case SimpleProfiler::Chunk:
                this->printChunks();
                break;
        }
    }

    void SimpleProfiler::printChunks() const {
        const static std::string dims[] = {"Overworld", "Nether", "The end"};
        TextBuilder builder;
        for (int i = 0; i < 3; i++) {
            auto &dim_data = this->chunkInfo.chunk_counter[i];
            if (!dim_data.empty()) {
                builder.sTextF(TextBuilder::AQUA | TextBuilder::BOLD, "-- %s --\n",
                               dims[i].c_str());

                std::vector<std::pair<trapdoor::TBlockPos2, double>> v;

                for (auto &kv : dim_data) {
                    assert(!kv.second.empty());
                    auto time = micro_to_mill(std::accumulate(kv.second.begin(), kv.second.end(),
                                                              static_cast<microsecond_t>(0))) /
                                static_cast<double>(kv.second.size());
                    v.emplace_back(kv.first, time);
                }

                auto sort_count = std::min(static_cast<long long>(v.size()), 5ll);
                std::partial_sort(v.begin(), v.begin() + sort_count, v.end(),
                                  [](const std::pair<trapdoor::TBlockPos2, double> &p1,
                                     const std::pair<trapdoor::TBlockPos2, double> &p2) {
                                      return p1.second > p2.second;
                                  });

                for (int j = 0; j < sort_count; j++) {
                    builder.text(" - ")
                        .sTextF(TextBuilder::GREEN, "[%d %d]   ", v[j].first.x * 16 + 8,
                                v[j].first.z * 16 + 8)
                        .textF("%.3f ms\n", v[j].second);
                }
            }
        }

        trapdoor::broadcastMessage(builder.get());
    }
    void SimpleProfiler::printPendingTicks() const {
        trapdoor::logger().debug("print Pending ticks info!!");
        const static std::string dims[] = {"Overworld", "Nether", "The end"};
        TextBuilder builder;
        for (int i = 0; i < 3; i++) {
            auto &pt_data = this->ptCounter[i];
            if (!pt_data.empty()) {
                builder.sTextF(TextBuilder::AQUA | TextBuilder::BOLD, "-- %s --\n",
                               dims[i].c_str());
                std::vector<std::pair<TBlockPos2, size_t>> v;
                for (auto &kv : pt_data) {
                    assert(!kv.second.empty());
                    v.emplace_back(kv);
                }

                std::sort(
                    v.begin(), v.end(),
                    [](const std::pair<TBlockPos2, size_t> &p1,
                       const std::pair<TBlockPos2, size_t> &p2) { return p1.second > p2.second; });

                if (v.size() > 10) v.resize(10);
                for (auto &item : v) {
                    builder.text(" - ")
                        .sTextF(TextBuilder::GREEN, "%s   ", item.first.toString().c_str())
                        .textF("%d\n", item.second);
                }
            }
        }
        TextBuilder bu;
        trapdoor::broadcastMessage(bu.get() + builder.get());
    }
    void SimpleProfiler::printBasics() const {
        /*
  ServerLevel::tick
     - Redstone
        - Dimension::tickRedstone(shouldUpdate,cacheValue,evaluate)
        - pendingUpdate
        - pendingRemove
        - pendingAdd
     - Dimension::tick(chunk load/village)
     - entitySystem
     - Level::tick
        - LevelChunk::Tick
            - blockEntities
            - randomChunk
            - Actor::tick(non global)
    */

        const double divide = 1000.0 * static_cast<double>(totalRound);
        trapdoor::logger().debug("divide = {}", divide);
        auto cf = [divide](microsecond_t time) { return static_cast<float>(time) * 1.0f / divide; };
        auto mspt = cf(gameSessionTickTime);
        int tps = mspt <= 50 ? 20 : static_cast<int>(1000.0 / mspt);
        auto res = fmt::format(
            "- MSPT: {:.3f} ms TPS: {} Chunks: {}\n"
            "- Trapdoor: {:.3f} ms\n"
            "- Redstone: {:.3f} ms\n"
            "   - Signal: {:.3f} ms\n"
            "   - Add: {:.3f} ms\n"
            "   - Update: {:.3f} ms\n"
            "   - Remove: {:.3f} ms\n"
            "- EntitySystems: {:.3f} ms\n"
            "- Chunk (un)load & village: {:.3f} ms\n"
            "- Chunk tick: {:.3f} ms\n"
            "   - BlockEntities: {:.3f} ms\n"
            "   - RandomTick: {:.3f} ms\n"
            "   - PendingTick: {:.3f} ms\n",

            /*summary*/
            mspt, tps, this->chunkInfo.getChunkNumber(),
            /*trapdoor*/
            cf(trapdoorSessionTickTime),
            /*redstone*/
            cf(redstoneInfo.sum()),
            cf(redstoneInfo.signalUpdate),  //
            cf(redstoneInfo.pendingAdd), cf(redstoneInfo.pendingUpdate),
            cf(redstoneInfo.pendingRemove),  //
            /*entities system & dimension*/
            cf(entitySystemTickTime), cf(dimensionTickTime),  //
            /*chunks*/
            cf(static_cast<microsecond_t>(chunkInfo.totalTickTime)),
            cf(chunkInfo.blockEntitiesTickTime), cf(chunkInfo.randomTickTime),
            cf(chunkInfo.pendingTickTime));
        trapdoor::broadcastMessage(res);
    }

    void SimpleProfiler::printActor() const {
        const static std::string dims[] = {"Overworld", "Nether", "The end"};
        TextBuilder builder;
        double totalTime = 0.0;
        for (int i = 0; i < 3; i++) {
            auto &actor_data = this->actorInfo[i];
            if (!actor_data.empty()) {
                builder.sTextF(TextBuilder::AQUA | TextBuilder::BOLD, "-- %s --\n",
                               dims[i].c_str());
                std::vector<std::pair<std::string, EntityInfo>> v;
                for (auto &kv : actor_data) {
                    assert(!kv.second.empty());
                    v.emplace_back(kv);
                }

                std::sort(v.begin(), v.end(),
                          [](const std::pair<std::string, EntityInfo> &p1,
                             const std::pair<std::string, EntityInfo> &p2) {
                              return p1.second.time > p2.second.time;
                          });

                for (auto &item : v) {
                    builder.text(" - ")
                        .sTextF(TextBuilder::GREEN, "%s   ",
                                trapdoor::i18ActorName(item.first).c_str())
                        .textF(
                            "%.3f ms (%d)\n",
                            micro_to_mill(item.second.time) / static_cast<double>(this->totalRound),
                            item.second.count / totalRound);
                    totalTime +=
                        micro_to_mill(item.second.time) / static_cast<double>(this->totalRound);
                }
            }
        }
        TextBuilder bu;
        bu.text("Total ").num(totalTime).text(" ms\n");
        trapdoor::broadcastMessage(bu.get() + builder.get());
    }  // namespace trapdoor
}  // namespace trapdoor
