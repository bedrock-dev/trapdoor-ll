#ifndef TRAPDOOR_HOPPER_COUNTRT_H
#define TRAPDOOR_HOPPER_COUNTRT_H

// clang-format off
#include "Global.h"
#include <MC/Vec3.hpp>
#include <map>
#include <string>
#include <vector>
// clang-format on
#include "CommandHelper.h"

namespace tr {

    class CounterChannel {
        const size_t channel;                       //频道号
        std::map<std::string, size_t> counterList;  //数据
        size_t gameTick = 0;                        //游戏刻
       public:
        explicit CounterChannel(size_t ch) : channel(ch), gameTick(0) {}

        void reset();

        ActionResult print();

        void add(const std::string &itemName, size_t num);

        inline void tick() { ++gameTick; }
    };

    //漏斗频道管理器
    class HopperChannelManager {
        std::vector<CounterChannel> channels;
        bool enable = false;

       public:
        static const size_t TOTAL_CHANNEL_NUM;
        HopperChannelManager() {
            for (int i = 0; i < 16; i++) channels.emplace_back(i);
        }

        inline CounterChannel &getChannel(size_t ch) { return channels[ch]; }

        //更新计数器
        void tick();

        inline bool isEnable() const { return this->enable; }

        inline ActionResult setAble(bool able) {
            this->enable = able;
            if (this->enable) {
                return {"Hopper counter is enable", true};
            } else {
                return {"Hopper counter is disable", true};
            }
        }

        //打印某个频道
        ActionResult printChannel(size_t channel);

        //重置某个频道
        ActionResult resetChannel(size_t channel);

        void quickPrintData(const BlockPos &pos);
    };

}  // namespace tr

#endif