#include "HUDHelper.h"

#include <MC/Biome.hpp>
#include <MC/Block.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Brightness.hpp>
#include <MC/Level.hpp>
#include <MC/Player.hpp>

#include "CommandHelper.h"
#include "DataConverter.h"
#include "HopperCounter.h"
#include "MCTick.h"
#include "Msg.h"
#include "PlayerInfoAPI.h"
#include "Utils.h"

namespace tr {

    namespace {

        std::string buildHopperCounter(Player* player) {
            auto& hcm = tr::mod().hopper_channel_manager();
            if (!hcm.isEnable()) return "";
            TextBuilder b;
            auto pointBlock = reinterpret_cast<Actor*>(player)->getBlockFromViewVector();
            if (pointBlock.isNull()) return "";
            auto* block = pointBlock.getBlock();
            if (block->getId() == HopperChannelManager::HOPPER_COUNTER_BLOCK) {
                return tr::mod().hopper_channel_manager().getHUDdata(block->getVariant());
            }

            return "";
        }

        std::string buildBaseHud(Player* player) {
            TextBuilder b;
            auto pos = player->getPos();
            b.textF("X/Y/Z:  %.3f %.3f %.3f\n", pos.x, pos.y, pos.z);
            auto view = player->getViewVector(1.0);
            b.textF("View:  %.3f %.3f %.3f\n", view.x, view.y, view.z);
            TBlockPos p = fromVec3(pos).toBlockPos();
            auto cp = p.toChunkPos();
            auto coff = p.InChunkOffset();
            b.textF("Chunk:  [%d %d] in [%d %d]\n", coff.x, coff.z, cp.x, cp.z);
            auto& bs = player->getRegion();
            auto pointBlock = reinterpret_cast<Actor*>(player)->getBlockFromViewVector();
            auto pointPos = pointBlock.getPosition();
            auto block = pointBlock.getBlock();
            std::string name = "air";
            if (block) {
                name = tr::rmmc(block->getTypeName());
            }
            if (!pointBlock.isNull()) {
                b.textF("PointTo: %d %d %d  %s\n", pointPos.x, pointPos.y, pointPos.z,
                        name.c_str());
            } else {
                b.textF("PointTo: Null\n");
            }
            auto rb = bs.getRawBrightness(pointPos + BlockPos(0, 1, 0), true, true);
            auto bright = (uint32_t) * reinterpret_cast<unsigned char*>(&rb);
            b.textF("Light: %u\n", bright);
            b.textF("Biome: %d\n", bs.getBiome(pointPos).getBiomeType());
            return b.get();
        }
        std::string buildMsptHud() {
            TextBuilder builder;
            auto mspt = tr::getMeanMSPT();
            auto tps = 1000.0 / mspt;
            if (tps > 20.0) tps = 20.0;
            builder.textF("MSPT: %.3f TPS: %.1f\n", mspt, tps);
            return builder.get();
        }

        HUDInfoType getTypeFromString(const std::string& str) {
            if (str == "base") return HUDInfoType::Base;
            if (str == "hoppercounter") return HUDInfoType::Counter;
            if (str == "village") return HUDInfoType::Vill;
            if (str == "mspt") return HUDInfoType::Mspt;
            return HUDInfoType::Unknown;
        }

    }  // namespace
    void HUDHelper::tick() {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 30;
        if (refresh_time != 1) return;
        for (auto& info : this->playerInfos) {
            auto* p = Global<Level>->getPlayer(info.first);
            if (p && info.second.enbale) {
                std::string s;
                auto& cfg = info.second.config;
                if (cfg[HUDInfoType::Base]) {
                    s += buildBaseHud(p);
                }
                if (cfg[HUDInfoType::Mspt]) {
                    s += buildMsptHud();
                }
                if (cfg[HUDInfoType::Vill]) {
                    s += "Village\n";
                }
                if (cfg[HUDInfoType::Counter]) {
                    s += buildHopperCounter(p);
                }
                p->sendText(s, TextType::TIP);
            }
        }
    }

    ActionResult HUDHelper::modifyPlayerInfo(const std::string& playerName, const std::string& item,
                                             int op) {
        auto type = getTypeFromString(item);
        if (type == HUDInfoType::Unknown) {
            return {"Unknown type", false};
        }
        this->playerInfos[playerName].config[type] = op;
        return {"success", true};
    }

    ActionResult HUDHelper::setAblePlayer(const std::string& playerName, bool able) {
        this->playerInfos[playerName].enbale = able;
        return {"success", true};
    }
}  // namespace tr
