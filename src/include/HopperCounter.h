#ifndef TRAPDOOR_HOPPER_COUNTER_H
#define TRAPDOOR_HOPPER_COUNTER_H

// clang-format off
#include <vcruntime.h>
#include "Global.h"
#include <mc/Vec3.hpp>
#include <mc/Player.hpp>
#include <map>
#include <string>
#include <vector>
// clang-format on
#include "CommandHelper.h"

namespace trapdoor {
    class CounterChannel {
        const size_t channel;                       // 频道号
        std::map<std::string, size_t> counterList;  // 数据
        std::map<size_t, size_t> gtCounter;         // 最近1200gt的数据
        size_t gameTick = 0;                        // 游戏刻

       public:
        explicit CounterChannel(size_t ch) : channel(ch), gameTick(0) {}

        ActionResult reset();

        std::string info(bool simple = false);

        void add(const std::string &itemName, size_t num);

        inline void tick() { ++gameTick; }
    };

    // 漏斗频道管理器
    class HopperChannelManager {
        std::vector<CounterChannel> channels;

       public:
        const static std::unordered_map<std::string, int> HOPPER_COUNTER_MAP;

        HopperChannelManager() {
            for (int i = 0; i < 16; i++) channels.emplace_back(i);
        }

        inline CounterChannel &getChannel(size_t ch) { return channels[ch]; }

        // 更新计数器
        void tick();

        [[nodiscard]] inline bool isEnable() const;

        ActionResult modifyChannel(Player *player, int channel, int opt);

        ActionResult quickModifyChannel(Player *player, const BlockPos &pos, int opt);

        std::string getHUDData(int channel);

        inline void clearAllData() {
            for (auto &ch : this->channels) {
                ch.reset();
            }
        }
    };

}  // namespace trapdoor

#endif