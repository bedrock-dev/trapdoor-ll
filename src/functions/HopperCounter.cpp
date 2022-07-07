#include "HopperCounter.h"

#include <MC/Block.hpp>
#include <MC/BlockActor.hpp>
#include <MC/BlockSource.hpp>
#include <MC/I18n.hpp>
#include <MC/Item.hpp>
#include <MC/ItemStackBase.hpp>
#include <stdexcept>

#include "DataConverter.h"
#include "HookAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"

namespace tr {

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
        tr::TextBuilder builder;

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

}  // namespace tr

/*
?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z
*/

THook(void, "?setItem@HopperBlockActor@@UEAAXHAEBVItemStack@@@Z", void *self, unsigned int index,
      ItemStackBase *itemStack) {
    auto &hcm = tr::mod().getHopperChannelManager();
    if (!hcm.isEnable()) {
        original(self, index, itemStack);
        return;
    }

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

    auto dir = tr::facingToBlockPos(static_cast<tr::TFACING>(block->getVariant()));
    auto pointPos = BlockPos(pos.x + dir.x, pos.y + dir.y, pos.z + dir.z);
    auto &pointBlock = nearest->getRegion().getBlock(pointPos);
    if (pointBlock.getId() != 236) {  // 混凝土
        original(self, index, itemStack);
        return;
    }

    auto ch = pointBlock.getVariant();
    if (ch < 0 || ch > 15 || itemStack->getName().empty()) {
        original(self, index, itemStack);
        return;
    }

    tr::mod().getHopperChannelManager().getChannel(ch).add(itemStack->getName(),
                                                           itemStack->getCount());
}
