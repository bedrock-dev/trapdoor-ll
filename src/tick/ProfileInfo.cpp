//
// Created by xhy on 2022/5/17.
//

#include "ProfileInfo.h"
#include <numeric>

namespace tr {

    double MSPTInfo::mean() const {
        return this->values.empty() ? 0.0 :
               std::accumulate(values.begin(), values.end(), 0.0) /
               static_cast<double >(values.size());
    }

    void MSPTInfo::push(double value) {
        this->values.push_back(value);
        if (this->values.size() > 20) {
            this->values.pop_front();
        }
    }

    double MSPTInfo::min() const {
        if (values.empty()) {
            return 0;
        }
        auto min = values[0];
        for (auto v: values) {
            if (min > v)min = v;
        }
        return min;


    }

    double MSPTInfo::max() const {
        if (values.empty()) {
            return 0;
        }
        auto max = values[0];
        for (auto v: values) {
            if (max < v)max = v;
        }
        return max;
    }
}