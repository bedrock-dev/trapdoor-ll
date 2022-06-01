#include "HopperCounter.h"

#include <MC/Block.hpp>
#include <MC/BlockActor.hpp>
#include <MC/BlockSource.hpp>
#include <MC/ItemStackBase.hpp>
#include <stdexcept>

#include "DataConverter.h"
#include "HookAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"

namespace tr {

    const size_t HopperChannelManager::TOTAL_CHANNEL_NUM = 16;

    void HopperChannelManager::tick() {
        if (this->enable) {
            for (auto &channel : channels) {
                channel.tick();
            }
        }
    }

    ActionResult HopperChannelManager::printChannel(size_t channel) {
        if (channel < 0 || channel >= TOTAL_CHANNEL_NUM) {
            return {"err", false};
        } else {
            return getChannel(channel).print();
        }
    }

    ActionResult HopperChannelManager::resetChannel(size_t channel) {
        if (channel < 0 || channel > 15) {
            return {"err", false};
        } else {
            getChannel(channel).reset();
            return {"~", true};
        }
    }

    void HopperChannelManager::quickPrintData(const BlockPos &pos) {
        // if (!this->enable) return;
        // auto *block = player->getBlockSource()->getBlock(pos);
        // if (block->getLegacy()->getBlockID() !=
        //     HopperChannelManager::BLOCK_TYPE)
        //     return;
        // this->printChannel(player, block->getVariant());
    }

    void CounterChannel::add(const std::string &itemName, size_t num) {
        counterList[itemName] += num;
    }

    void CounterChannel::reset() {
        gameTick = 0;
        counterList.clear();
    }

    ActionResult CounterChannel::print() {
        size_t n = 0;
        for (const auto &i : this->counterList) {
            n += i.second;
        }

        if (this->gameTick == 0 || n == 0) {
            return {"no data in this channel", false};
        }
        std::string stringBuilder;
        tr::TextBuilder builder;
        builder.textF("Channel [%d]: Total %zu items in %d gt (%.3f min(s))\n",
                      channel, n, gameTick, gameTick / 1200.0f);

        for (const auto &i : counterList) {
            //   auto itemName = GetItemLocalName(i.first);
            builder.textF(" - %s: ", i.first.c_str())
                .sTextF(TextBuilder::GREEN, "%d", i.second)
                .text("(")
                .sTextF(TextBuilder::GREEN, "%.3f",
                        i.second * 1.0f / gameTick * 72000)
                .text("/hour)\n");
        }
        return {builder.get(), true};
    }

}  // namespace tr

/*
?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z
*/

THook(void, "?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z", void *self,
      unsigned int index, ItemStackBase *itemStack) {
    auto &ba = dAccess<BlockActor, -200>(self);
    auto *block = ba.getBlock();
    if (!block) {
        original(self, index, itemStack);
        return;
    }

    // get PointPOsition
    auto &pos = ba.getPosition();
    // try get player
    Player *nearest = nullptr;

    try {
        Global<Level>->forEachPlayer([&](Player &player) {
            auto &b = player.getRegion().getBlock(pos);
            if (&b == block) {
                tr::logger().debug("find {}", player.getRealName());
                nearest = &player;
                throw std::logic_error("");
            }
            return true;
        });
    } catch (std::exception &) {
    }

    if (!nearest) {
        original(self, index, itemStack);
        return;
    }

    auto dir =
        tr::facingToBlockPos(static_cast<tr::TFACING>(block->getVariant()));
    auto pointPos = BlockPos(pos.x + dir.x, pos.y + dir.y, pos.z + dir.z);
    auto &pointBlock = nearest->getRegion().getBlock(pointPos);
    if (pointBlock.getId() != 236) {
        original(self, index, itemStack);
        return;
    }

    tr::logger().debug("{} * {}", itemStack->getName(), itemStack->getCount());
}
