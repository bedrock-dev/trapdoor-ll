#include "HopperCounter.h"

#include <mc/Block.hpp>
#include <mc/BlockActor.hpp>
#include <mc/BlockSource.hpp>
#include <mc/ChunkPos.hpp>
#include <mc/Dimension.hpp>
#include <mc/I18n.hpp>
#include <mc/Item.hpp>
#include <mc/ItemStackBase.hpp>
#include <stdexcept>

#include "DataConverter.h"
#include "HookAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"
namespace trapdoor {

    namespace {

        BlockSource *getProperRegion(Block *block, const BlockPos &pos) {
            auto cp = fromBlockPos(pos).toChunkPos();
            for (int i = 0; i < 3; i++) {
                auto bs = Global<Level>->getDimension(1)->tryGetClosestPublicRegion({cp.x, cp.z});
                if (bs && &bs->getBlock(pos.x, pos.y, pos.z) == block) {
                    return bs;
                }
            }
            return nullptr;
        }
    }  // namespace

    const size_t HopperChannelManager::HOPPER_COUNTER_BLOCK = 236;
    void HopperChannelManager::tick() {
        if (this->enable) {
            for (auto &channel : channels) {
                channel.tick();
            }
        }
    }

    ActionResult HopperChannelManager::modifyChannel(size_t channel, int opt) {
        if (channel < 0 || channel > 15) {
            return {"Invalid channel number", false};
        } else {
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
        if (!player) return ErrorPlayerNeed();
        auto &bs = player->getRegion();
        auto &b = bs.getBlock(pos);
        if (b.getId() != HOPPER_COUNTER_BLOCK) {
            return {"", true};
        }
        auto ch = b.getVariant();
        return modifyChannel(ch, opt);
    }

    void HopperChannelManager::quickPrintData(const BlockPos &pos) {}

    std::string HopperChannelManager::getHUDData(size_t channel) {
        if (channel < 0 || channel > 15) return "";
        auto &ch = this->getChannel(channel);
        return ch.info();
    }

    void CounterChannel::add(const std::string &itemName, size_t num) {
        counterList[itemName] += num;
    }

    ActionResult CounterChannel::reset() {
        gameTick = 0;
        counterList.clear();
        return {"Channel cleaned", true};
    }

    std::string CounterChannel::info(bool simple) {
        size_t n = 0;
        for (const auto &i : this->counterList) {
            n += i.second;
        }

        if (this->gameTick == 0 || n == 0) {
            return "No data in this channel";
        }

        std::string stringBuilder;
        trapdoor::TextBuilder builder;

        if (!simple) {
            builder.text("Channel: ").sTextF(TB::BOLD | TB::WHITE, "%d \n", channel);
            builder.text("Total ")
                .num(n)
                .text(" in ")
                .num(gameTick)
                .text(" gt (")
                .num(static_cast<float>(gameTick) / 72000.0f)
                .text(" h)\n");
        } else {
            builder.textF("%d (%.1f h))\n", n, static_cast<float>(gameTick) / 72000.0f);
        }

        for (const auto &i : counterList) {
            builder.sText(TB::GRAY, " - ");
            builder.textF("%s:   ", i.first.c_str())
                .num(i.second)
                .text(" (")
                .num(static_cast<float>(i.second) * 1.0f / static_cast<float>(gameTick) * 72000)
                .text("/h)\n");
        }

        return builder.get();
    }

}  // namespace trapdoor

/*
?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z
*/

#define HOPPER_RET                    \
    original(self, index, itemStack); \
    return;

THook(void, "?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z", void *self, unsigned int index,
      ItemStackBase *itemStack) {
    auto &hcm = trapdoor::mod().getHopperChannelManager();

    if (!hcm.isEnable()) {
        HOPPER_RET
    }

    auto &ba = dAccess<BlockActor>(self, -200);
    auto pos = ba.getPosition();
    auto cp = trapdoor::fromBlockPos(pos).toChunkPos();
    auto bs = Global<Level>->getDimension(0)->tryGetClosestPublicRegion({cp.x, cp.z});

    auto block = ba.getBlock();
    if (!bs || !block) {
        trapdoor::logger().debug("can not found a BlockSource");
        HOPPER_RET
    }

    auto dir = trapdoor::facingToBlockPos(static_cast<trapdoor::TFACING>(block->getVariant()));
    auto pointPos = BlockPos(pos.x + dir.x, pos.y + dir.y, pos.z + dir.z);
    auto &pointBlock = bs->getBlock(pointPos);

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
    itemStack->setNull();
    HOPPER_RET;
}
