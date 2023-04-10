#include "HopperCounter.h"

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

#include "DataConverter.h"
#include "HookAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"

namespace trapdoor {

    const size_t HopperChannelManager::HOPPER_COUNTER_BLOCK = 236;

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
        if (b.getId() != HOPPER_COUNTER_BLOCK) {
            return {"", true};
        }

        auto ch = b.getVariant();
        return modifyChannel(player, ch, opt);
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
    }

    ActionResult CounterChannel::reset() {
        gameTick = 0;
        counterList.clear();
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

        //        } else {
        //            builder.textF("%d,(%.1f h))\n", n, static_cast<float>(gameTick) / 72000.0f);
        //        }

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
}

THook(void, "?tick@HopperBlockActor@@UEAAXAEAVBlockSource@@@Z", HopperBlockActor *self,
      BlockSource *region) {
    hopperRegion = region;
    original(self, region);
}

THook(void, "?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z", void *self, unsigned int index,
      ItemStackBase *itemStack) {
    auto &hcm = trapdoor::mod().getHopperChannelManager();
    if (!hcm.isEnable()) {
        HOPPER_RET
    }
    if (!hopperRegion) {
        trapdoor::logger().debug("Invalid Hopper region");
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

    if (pointBlock.getId() != trapdoor::HopperChannelManager::HOPPER_COUNTER_BLOCK) {  // 混凝土
        HOPPER_RET;
    }

    auto ch = pointBlock.getVariant();
    if (ch < 0 || ch > 15 || itemStack->getName().empty()) {
        HOPPER_RET;
    }

    trapdoor::logger().debug("{} ==> {}", itemStack->getName(), itemStack->getCount());
    trapdoor::mod().getHopperChannelManager().getChannel(ch).add(itemStack->getName(),
                                                                 itemStack->getCount());
    // trapdoor::logger().debug("set null {}", reinterpret_cast<uint64_t>(itemStack));
    itemStack->remove(64);
    // trapdoor::logger().debug("remove finish");
    //  HOPPER_RET;
}
