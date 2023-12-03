#include "HUDHelper.h"

#include <vcruntime.h>

#include <mc/BaseCircuitComponent.hpp>
#include <mc/Biome.hpp>
#include <mc/Block.hpp>
#include <mc/BlockSource.hpp>
#include <mc/Brightness.hpp>
#include <mc/Dimension.hpp>
#include <mc/Level.hpp>
#include <mc/Player.hpp>
#include <unordered_map>

#include "CommandHelper.h"
#include "Configuration.h"
#include "DataConverter.h"
#include "HopperCounter.h"
#include "MCTick.h"
#include "Msg.h"
#include "Particle.h"
#include "PlayerInfoAPI.h"
#include "TrAPI.h"
#include "TrapdoorMod.h"
#include "Utils.h"
#include "VillageHelper.h"

namespace trapdoor {
    namespace {

        std::string buildVillagerInfo(Player *player) {
            auto act = reinterpret_cast<Actor *>(player)->getActorFromViewVector(5.25);
            return trapdoor::mod().getVillageHelper().getVillagerInfo(act, false);
        }

        std::string buildRedstoneInfo(Player *player) {
            auto pointBlock = reinterpret_cast<Actor *>(player)->getBlockFromViewVector();
            if (pointBlock.isNull()) return "Signal: -";

            auto &cs = player->getDimension().getCircuitSystem();
            auto &graph = getCircuitSceneGraph(&cs);
            auto comp = graph.getBaseComponent(pointBlock.getPosition());

            std::string redstone_signal = "-";
            std::string container_signal = "-";

            if (comp) {
                redstone_signal = std::to_string(comp->getStrength());
            }

            auto *block = pointBlock.getBlock();
            if (block && block->hasComparatorSignal()) {
                container_signal = std::to_string(
                    block->getComparatorSignal(player->getRegion(), pointBlock.getPosition(),
                                               static_cast<unsigned char>(FaceID::East)));
            }

            return fmt::format("Signal: {} / {}", redstone_signal, container_signal);
        }

        std::string buildHopperCounter(Player *player) {
            return trapdoor::mod().getHopperChannelManager().getHUDData(
                trapdoor::mod().getUserConfig().getActiveHopperChannel(player->getRealName()));
        }

        std::string buildContainerInfo(Player *player) {
            //            TextBuilder b;
            //            auto pointBlock =
            //            reinterpret_cast<Actor*>(player)->getBlockFromViewVector(); if
            //            (pointBlock.isNull()) return ""; auto* block = pointBlock.getBlock();
            //            if(!block || !block->isContainerBlock())return  "";
            //           // block->getComparatorSignal();
            return "";
        }

        // 挪动到MicroTicking里面
        std::string buildChunkOffset(Player *player) {
            const int R = 7;
            auto chunkStatus =
                std::vector<std::vector<int>>(R * 2 + 1, std::vector<int>(R * 2 + 1, -1));
            auto &tickingList = trapdoor::mod().getMicroTickingManager().tickingList(
                player->getDimension().getDimensionId());

            std::map<TBlockPos2, int> cpMap;
            for (int i = 0; i < tickingList.size(); i++) {
                cpMap[tickingList[i]] = i;
            }

            auto p = player->getBlockPos();
            auto cp = TBlockPos(p.x, p.y, p.z).toChunkPos();
            for (int i = -R; i <= R; i++) {
                for (int j = -R; j <= R; j++) {
                    auto target = TBlockPos2(cp.x + i, cp.z + j);
                    const auto it = cpMap.find(target);
                    if (it != cpMap.end()) {
                        chunkStatus[i + R][j + R] = it->second;
                    } else {
                        // trapdoor::logger().debug(
                        //     "{} {} does not exist in ticking map({} chunks {} tickings)",
                        //     target.x, target.z, cpMap.size(), tickingList.size());
                    }
                }
            }

            std::string res;
            for (auto &line : chunkStatus) {
                std::string buffer;
                for (auto &cp : line) {
                    buffer += cp == -1 ? "   " : std::to_string(cp) + " ";
                }
                res += (buffer + "\n");
            }
            return res;
        }

        std::string buildBaseHud(Player *player) {
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
            auto &bs = player->getRegion();
            auto pointBlock = reinterpret_cast<Actor *>(player)->getBlockFromViewVector();
            auto pointPos = pointBlock.getPosition();

            auto rb = bs.getRawBrightness(pointPos + BlockPos(0, 1, 0), true, true);
            auto bright = (uint32_t) * reinterpret_cast<unsigned char *>(&rb);
            if (!pointBlock.isNull()) {
                b.textF("Block: %d %d %d  Light: %u\n", pointPos.x, pointPos.y, pointPos.z, bright);
            } else {
                b.text("Block: None\n");
            }
            auto &delta = player->getPosDeltaNonConst();
            b.textF("Speed: %.4lf (%.2lf %.2lf %.2lf)\n", delta.length() * 20, delta.x * 20,
                    delta.y * 20, delta.z * 20);
            auto &biome = bs.getBiome(pos);
            b.textF("Biome: %s (%d)\n", biome.getName().c_str(), biome.getId());
            return b.removeEndl().get();
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
            return builder.removeEndl().get();
        }
        std::string buildSpawnCap() {
            auto cap = dAccess<int, 64 * 4>(&Global<Level>->getSpawner());
            auto color = cap >= 200 ? trapdoor::TextBuilder::RED : trapdoor::TextBuilder::GREEN;
            TextBuilder builder;
            builder.text("Global Cap: ").sTextF(color, "%d", cap);
            return builder.removeEndl().get();
        }
    }  // namespace

    HUDItemType getHUDTypeFromString(const string &str) {
        if (str == "base") return HUDItemType::Base;
        if (str == "hopper") return HUDItemType::Counter;
        if (str == "village") return HUDItemType::Vill;
        if (str == "redstone") return HUDItemType::Redstone;
        if (str == "mspt") return HUDItemType::Mspt;
        if (str == "chunk") return HUDItemType::Chunk;
        if (str == "container") return HUDItemType::Cont;
        if (str == "cap") return HUDItemType::GlobalCap;
        return HUDItemType::Unknown;
    }

    std::string getStringFromHUDType(HUDItemType t) {
        switch (t) {
            case Unknown:
                return "unknown";
            case Base:
                return "base";
            case Mspt:
                return "mspt";
            case Vill:
                return "village";
            case Redstone:
                return "redstone";
            case Counter:
                return "hopper";
            case Chunk:
                return "chunk";
            case Cont:
                return "container";
            case LEN:
                return "unknown";
            case GlobalCap:
                return "cap";
                break;
        }
        return "unknown";
    }

    // 下面是成员函数

    std::vector<std::string> HUDHelper::getHUDItemStringList() {
        std::vector<std::string> hudItems;
        for (int i = 1; i < HUDItemType::LEN; i++) {  // 0是unknown不列入
            hudItems.push_back(getStringFromHUDType(static_cast<HUDItemType>(i)));
        }
        return hudItems;
    }

    void HUDHelper::tickChunk() const {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 15;
        if (refresh_time != 1) return;

        auto &playerCfg = trapdoor::mod().getUserConfig().getPlayerData();
        for (auto &info : playerCfg) {
            auto *p = Global<Level>->getPlayer(info.first);
            if (p && info.second.hud && info.second.hud_config[HUDItemType::Chunk]) {
                auto pos = p->getPos().toBlockPos();
                trapdoor::drawChunkSurface(fromBlockPos(pos).toChunkPos(),
                                           (int)p->getDimension().getDimensionId());
            }
        }
    }

    void HUDHelper::tick() {
        if (!trapdoor::mod().getConfig().getGlobalFunctionConfig().hud) return;
        this->tickChunk();
        static int refresh_time = 0;
        refresh_time =
            (refresh_time + 1) % trapdoor::mod().getConfig().getBasicConfig().hudRefreshFreq;
        if (refresh_time != 1) return;
        auto ps = [](const std::string &s) { return s.empty() ? s : s + "\n"; };
        // 遍历所有的有配置文件的信息（想有显示就必须add，add就会创建表项，因此不会出现玩家不在列表的选项）
        auto playerInfos = trapdoor::mod().getUserConfig().getPlayerData();
        for (auto &info : playerInfos) {
            auto *p = Global<Level>->getPlayer(info.first);
            if (p && info.second.hud) {
                std::string s;
                auto &cfg = info.second.hud_config;
                if (cfg[HUDItemType::Base]) {
                    s += ps(buildBaseHud(p));
                }

                if (cfg[HUDItemType::Mspt]) {
                    s += ps(buildMsptHud());
                }
                if (cfg[HUDItemType::Redstone]) {
                    s += ps(buildRedstoneInfo(p));
                }
                if (cfg[static_cast<size_t>(HUDItemType::Vill)]) {
                    s += ps(buildVillagerInfo(p));
                }
                if (cfg[HUDItemType::Counter]) {
                    s += ps(buildHopperCounter(p));
                }
                if (cfg[HUDItemType::Cont]) {
                    s += ps(buildContainerInfo(p));
                }
                if (cfg[HUDItemType::GlobalCap]) {
                    s += ps(buildSpawnCap());
                }
                if (cfg[HUDItemType::Chunk]) {
                    s += ps(buildChunkOffset(p));
                }

                while (!s.empty() && s.back() == '\n') {
                    s.pop_back();
                }
                //  trapdoor::logger().debug("Send HUD: [{}]", s);
                p->sendText(s, TextType::TIP);
            }
        }
    }

    ActionResult HUDHelper::modifyPlayerInfo(const std::string &playerName, const std::string &item,
                                             int op) {
        auto type = getHUDTypeFromString(item);
        if (type == HUDItemType::Unknown) {
            return ErrorMsg("hud.error.unknown-type");
        }

        trapdoor::mod().getUserConfig().setHUD(playerName, type, op == 1);
        return OperationSuccess();
    }

}  // namespace trapdoor
