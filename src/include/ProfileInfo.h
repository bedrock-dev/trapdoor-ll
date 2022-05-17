//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_PROFILEINFO_H
#define TRAPDOOR_PROFILEINFO_H

#include <deque>


namespace tr {

    struct MSPTInfo {
        std::deque<double> values;

        double mean() const;

        void push(double value);

        double min() const;

        double max() const;
    };
}


#endif //TRAPDOOR_PROFILEINFO_H
