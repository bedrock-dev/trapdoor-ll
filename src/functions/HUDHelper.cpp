#include "HUDHelper.h"

#include <mc/BaseCircuitComponent.hpp>
#include <mc/Biome.hpp>
#include <mc/Block.hpp>
#include <mc/BlockSource.hpp>
#include <mc/Brightness.hpp>
#include <mc/Dimension.hpp>
#include <mc/Level.hpp>
#include <mc/Player.hpp>

#include "CommandHelper.h"
#include "Configuration.h"
#include "DataConverter.h"
#include "HopperCounter.h"
#include "MCTick.h"
#include "Msg.h"
#include "Particle.h"
#include "PlayerInfoAPI.h"
#include "TrAPI.h"
#include "Utils.h"
#include "VillageHelper.h"
namespace trapdoor {
    namespace {

        std::string buildVillagerInfo(Player* player) {
            auto act = reinterpret_cast<Actor*>(player)->getActorFromViewVector(5.25);
            return trapdoor::mod().getVillageHelper().getVillagerInfo(act, false);
        }

        std::string buildRedstoneInfo(Player* player) {
            auto pointBlock = reinterpret_cast<Actor*>(player)->getBlockFromViewVector();
            if (pointBlock.isNull()) return "Signal: -\n";

            auto& cs = player->getDimension().getCircuitSystem();
            auto& graph = getCircuitSceneGraph(&cs);
            auto comp = graph.getBaseComponent(pointBlock.getPosition());

            std::string redstone_signal = "-";
            std::string container_signal = "-";

            if(comp){
                redstone_signal = std::to_string(comp->getStrength());
            }

            auto *block = pointBlock.getBlock();
            if(block && block->hasComparatorSignal()){

                container_signal =  std::to_string(block->getComparatorSignal(player->getRegion(),pointBlock.getPosition(),
                                                                        static_cast<unsigned  char >(FaceID::East)));
            }

            return fmt::format("Signal: {} / {}\n",redstone_signal,container_signal);
        }

        std::string buildHopperCounter(Player* player) {
            auto& hcm = trapdoor::mod().getHopperChannelManager();
            if (!hcm.isEnable()) return "";
            TextBuilder b;
            auto pointBlock = reinterpret_cast<Actor*>(player)->getBlockFromViewVector();
            if (pointBlock.isNull()) return "";
            auto* block = pointBlock.getBlock();
            if (block->getId() == HopperChannelManager::HOPPER_COUNTER_BLOCK) {
                return trapdoor::mod().getHopperChannelManager().getHUDData(block->getVariant());
            }

            return "";
        }

        std::string  buildContainerInfo(Player * player){
//            TextBuilder b;
//            auto pointBlock = reinterpret_cast<Actor*>(player)->getBlockFromViewVector();
//            if (pointBlock.isNull()) return "";
//            auto* block = pointBlock.getBlock();
//            if(!block || !block->isContainerBlock())return  "";
//           // block->getComparatorSignal();
            return  "";
        }



        std::string buildBaseHud(Player* player) {
            TextBuilder b;
            b.textF("Tick: %zu\n", Global<Level>->getCurrentServerTick().t);
            auto pos = player->getPos();
            b.textF("X/Y/Z:  %.3f %.3f %.3f\n", pos.x, pos.y, pos.z);
            auto view = player->getViewVector(1.0);
            b.textF("View:   %.3f %.3f %.3f\n", view.x, view.y, view.z);
            TBlockPos p = fromVec3(pos).toBlockPos();
            auto cp = p.toChunkPos();
            auto coff = p.InChunkOffset();
            b.textF("Chunk:  [%d %d] in [%d %d]\n", coff.x, coff.z, cp.x, cp.z);
            auto& bs = player->getRegion();
            auto pointBlock = reinterpret_cast<Actor*>(player)->getBlockFromViewVector();
            auto pointPos = pointBlock.getPosition();

            auto rb = bs.getRawBrightness(pointPos + BlockPos(0, 1, 0), true, true);
            auto bright = (uint32_t) * reinterpret_cast<unsigned char*>(&rb);
            if (!pointBlock.isNull()) {
                b.textF("Block: %d %d %d  Light: %u\n", pointPos.x, pointPos.y, pointPos.z, bright);
            } else {
                b.text("Block: None\n");
            }
            auto& biome = bs.getBiome(pos);
            b.textF("Biome: %s (%d)\n", getBiomeName(&biome).c_str(), biome.getBiomeType());
            return b.get();
        }

        std::string buildMsptHud() {
            TextBuilder builder;
            auto mspt = trapdoor::getMeanMSPT();
            auto tps = 1000.0 / mspt;
            if (tps > 20.0) tps = 20.0;
            auto color = mspt <= 50 ? TextBuilder::GREEN : TextBuilder::RED;
            builder.text("MSPT: ")
                .sTextF(color, "%.3f", mspt)
                .text(" TPS: ")
                .sTextF(color, "%.1f", tps)
                .text("\n");
            return builder.get();
        }

        HUDInfoType getTypeFromString(const std::string& str) {
            if (str == "base") return HUDInfoType::Base;
            if (str == "hopper") return HUDInfoType::Counter;
            if (str == "village") return HUDInfoType::Vill;
            if (str == "redstone") return HUDInfoType::Redstone;
            if (str == "mspt") return HUDInfoType::Mspt;
            if (str == "chunk") return HUDInfoType::Chunk;
            if (str == "container") return HUDInfoType::Cont;
            return HUDInfoType::Unknown;
        }

    }  // namespace
    void HUDHelper::tickChunk() {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 15;
        if (refresh_time != 1) return;
        for (auto& info : this->playerInfos) {
            auto* p = Global<Level>->getPlayer(info.first);
            if (p && info.second.enable && info.second.config[HUDInfoType::Chunk]) {
                auto pos = p->getPos().toBlockPos();
                trapdoor::drawChunkSurface(fromBlockPos(pos).toChunkPos(),
                                           (int)p->getDimension().getDimensionId());
            }
        }
    }
    void HUDHelper::tick() {
        if (!this->enable) return;
        this->tickChunk();
        static int refresh_time = 0;
        refresh_time =
            (refresh_time + 1) % trapdoor::mod().getConfig().getBasicConfig().hudRefreshFreq;
        if (refresh_time != 1) return;
        for (auto& info : this->playerInfos) {
            auto* p = Global<Level>->getPlayer(info.first);
            if (p && info.second.enable) {
                std::string s;
                auto& cfg = info.second.config;
                if (cfg[HUDInfoType::Base]) {
                    s += buildBaseHud(p);
                }
                if (cfg[HUDInfoType::Mspt]) {
                    s += buildMsptHud();
                }
                if (cfg[HUDInfoType::Redstone]) {
                    s += buildRedstoneInfo(p);
                }
                if (cfg[HUDInfoType::Vill]) {
                    s += buildVillagerInfo(p);
                }
                if (cfg[HUDInfoType::Counter]) {
                    s += buildHopperCounter(p);
                }
                if(cfg[HUDInfoType::Cont]){
                    s += buildContainerInfo(p);
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
        return {"Success", true};
    }

    ActionResult HUDHelper::setAblePlayer(const std::string& playerName, bool able) {
        if (!this->enable) {
            return {"This function is disabled by Operator", false};
        }
        this->playerInfos[playerName].enable = able;
        return {"Success", true};
    }

}  // namespace trapdoor
