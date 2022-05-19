//
// Created by xhy on 2022/5/17.
//

#include "ProfileInfo.h"

#include <numeric>

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

    void NormalProfiler::Reset(NormalProfiler::Type type) {
        this->type = type;
        this->chunk_info.reset();
    }

    void NormalProfiler::Start(size_t round, NormalProfiler::Type type) {
        this->Reset(type);
        this->profiling = true;
        this->current_round = 0;
        this->total_round = round;
    }

    void NormalProfiler::Stop() {
        logger().info(fmt::format(
            "mspt: {}",
            tr::micro_to_mill(this->server_level_tick_time / total_round)));
        this->profiling = false;
        this->Reset(Normal);
    }
}  // namespace tr