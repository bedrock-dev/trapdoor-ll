#include "HopperCounter.h"

#include <minwindef.h>
#include <vcruntime.h>
#include <winnt.h>

#include <mc/Block.hpp>
#include <mc/BlockActor.hpp>
#include <mc/BlockSource.hpp>
#include <mc/ChunkPos.hpp>
#include <mc/Dimension.hpp>
#include <mc/HopperBlockActor.hpp>
#include <mc/I18n.hpp>
#include <mc/Item.hpp>
#include <mc/ItemStackBase.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "DataConverter.h"
#include "HookAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"
#include "Utils.h"

namespace trapdoor {

    const std::unordered_map<std::string, int> HopperChannelManager::HOPPER_COUNTER_MAP{
        {"white_concrete", 0},  {"light_gray_concrete", 1}, {"gray_concrete", 2},
        {"black_concrete", 3},  {"brown_concrete", 4},      {"red_concrete", 5},
        {"orange_concrete", 6}, {"yellow_concrete", 7},     {"lime_concrete", 8},
        {"green_concrete", 9},  {"cyan_concrete", 10},      {"light_blue_concrete", 11},
        {"blue_concrete", 12},  {"purple_concrete", 13},    {"magenta_concrete", 14},
        {"pink_concrete", 15}};

    void HopperChannelManager::tick() {
        if (this->isEnable()) {
            for (auto &channel : channels) {
                channel.tick();
            }
        }
    }

    ActionResult HopperChannelManager::modifyChannel(Player *player, int channel, int opt) {
        if (!this->isEnable()) {
            return ErrorMsg("hopper.error.disable");
        }

        if (channel < 0 || channel > 15) {
            return ErrorMsg("hopper.error.invalid-channel");
        } else {
            if (player) {
                // 添加到用户缓存中
                trapdoor::mod().getUserConfig().setActiveHopperChannel(player->getRealName(),
                                                                       channel);
            }

            auto &ch = this->getChannel(channel);
            if (opt == 0) {
                return {ch.info(), true};
            } else {
                return ch.reset();
            }
        }
    }

    ActionResult HopperChannelManager::quickModifyChannel(Player *player, const BlockPos &pos,
                                                          int opt) {
        if (!this->isEnable()) {
            return ErrorMsg("hopper.error.disable");
        }
        if (!player) return ErrorPlayerNeed();
        auto &bs = player->getRegion();
        auto &b = bs.getBlock(pos);

        auto bName = trapdoor::rmmc(b.getName().getString());
        auto it = HopperChannelManager::HOPPER_COUNTER_MAP.find(bName);

        if (it == HopperChannelManager::HOPPER_COUNTER_MAP.end()) {
            return {"", true};
        }

        return modifyChannel(player, it->second, opt);
    }

    std::string HopperChannelManager::getHUDData(int channel) {
        if (!this->isEnable()) return "";
        if (channel < 0 || channel > 15) return "";
        auto &ch = this->getChannel(channel);
        return ch.info(true);
    }

    bool HopperChannelManager::isEnable() const {
        return trapdoor::mod().getConfig().getGlobalFunctionConfig().hopperCounter;
    }

    void CounterChannel::add(const std::string &itemName, size_t num) {
        counterList[itemName] += num;
        gtCounter[gameTick] += num;
        // 维护最近一分钟的数据
        while (!gtCounter.empty()) {
            auto it = gtCounter.begin();
            if (gameTick - it->first >= 1200) {
                gtCounter.erase(it);
            } else {
                break;
            }
        }
    }

    ActionResult CounterChannel::reset() {
        gameTick = 0;
        counterList.clear();
        gtCounter.clear();
        return SuccessMsg("hopper.info.channel-cleaned");
    }

    std::string CounterChannel::info(bool simple) {
        size_t n = 0;
        for (const auto &i : this->counterList) {
            n += i.second;
        }

        if (this->gameTick == 0 || n == 0) {
            return tr("hopper.info.no-data");
        }

        std::string stringBuilder;
        trapdoor::TextBuilder builder;

        builder.text("Channel: ").sTextF(TB::BOLD | TB::WHITE, "%d \n", channel);

        size_t gtTotal = 0;
        for (auto &kv : this->gtCounter) {
            gtTotal += kv.second;
        }

        //        if (!simple) {
        //            builder.text("Total ");
        //        }
        //
        // total items number
        builder
            .num(n)
            // total speed
            .text(" items (")
            .num(static_cast<float>(n) * 1.0f / static_cast<float>(gameTick) * 72000)
            .text("/h)")
            // time
            .text(" in ")
            .num(gameTick)
            // time in hour
            .text(" gt (")
            .num(static_cast<float>(gameTick) / 72000.0f)
            .text(" h)\n");

        builder
            .num(gtTotal)
            // 最近一分钟的瞬时速度
            .text(" items (")
            .num(static_cast<float>(gtTotal) * 1.0f / static_cast<float>(1200) * 72000)
            .text("/h)\n");

        for (const auto &i : counterList) {
            builder.sText(TB::GRAY, " - ");
            builder.textF("%s:   ", i.first.c_str())
                .num(i.second)
                .text(" (")
                .num(static_cast<float>(i.second) * 1.0f / static_cast<float>(gameTick) * 72000)
                .text("/h)\n");
        }

        return builder.removeEndl().get();
    }

}  // namespace trapdoor

/*
?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z
*/

#define HOPPER_RET                    \
    original(self, index, itemStack); \
    return;

namespace {
    BlockSource *hopperRegion;

}  // namespace

THook(void, "?tick@HopperBlockActor@@UEAAXAEAVBlockSource@@@Z", HopperBlockActor *self,
      BlockSource *region) {
    hopperRegion = region;
    original(self, region);
}

THook(void, "?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z", HopperBlockActor *self,
      unsigned int index, ItemStackBase *itemStack) {
    auto &hcm = trapdoor::mod().getHopperChannelManager();
    if (!hcm.isEnable()) {
        HOPPER_RET
    }
    if (!hopperRegion) {
        trapdoor::logger().debug("Invalid Hopper region");
        HOPPER_RET
    }

    auto &ba = dAccess<BlockActor>(self, -200);
    auto pos = ba.getPosition();

    auto block = ba.getBlock();
    if (!block) {
        trapdoor::logger().debug("Invalid Block Actor");
        HOPPER_RET
    }

    auto dir = trapdoor::facingToBlockPos(static_cast<trapdoor::TFACING>(block->getVariant()));
    auto &pointBlock = hopperRegion->getBlock(pos + BlockPos(dir.x, dir.y, dir.z));

    auto blockName = trapdoor::rmmc(pointBlock.getName().getString());
    auto it = trapdoor::HopperChannelManager::HOPPER_COUNTER_MAP.find(blockName);
    if (it == trapdoor::HopperChannelManager::HOPPER_COUNTER_MAP.end()) {  // 混凝土
        HOPPER_RET;
    }

    auto ch = it->second;
    if (ch < 0 || ch > 15 || itemStack->getName().empty()) {
        HOPPER_RET;
    }

    // trapdoor::logger().debug("{} ==> {}", itemStack->getName(), itemStack->getCount());
    trapdoor::mod().getHopperChannelManager().getChannel(ch).add(itemStack->getName(),
                                                                 itemStack->getCount());
    // trapdoor::logger().debug("set null {}", reinterpret_cast<uint64_t>(itemStack));
    itemStack->remove(itemStack->getCount());
    // trapdoor::logger().debug("remove finish");
    //  HOPPER_RET;
}
