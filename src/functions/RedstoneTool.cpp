//
// Created by xhy on 2022/8/26.
//

// clang-format off
#include "Global.h"
// clang-format on

//clang-format on
//clang-format off
#include "RedstoneTool.h"

#include <mc/BaseCircuitComponent.hpp>
#include <mc/Block.hpp>
#include <mc/BlockActor.hpp>
#include <mc/Brightness.hpp>
#include <mc/Dimension.hpp>
#include <mc/Material.hpp>
#include <unordered_map>
#include <vector>

#include "CommandHelper.h"
#include "Msg.h"
#include "Particle.h"
#include "TBlockPos.h"
#include "TrAPI.h"
namespace trapdoor {
    namespace {
        struct ComponentItem {
            BaseCircuitComponent *mComponent = nullptr;  // 0 * 4 - 1 * 4
            int mDampening = 0;                          // 2 * 4
            BlockPos mPos;                               // 3 * 4 - 5 * 4
            unsigned char facing{};                      // 6 * 4
            bool mDirectlyPowered = false;               // 6* 4
            int mData = 0;                               // 7*4
        };

        struct PendingEntry {
            std::unique_ptr<BaseCircuitComponent> mComponent;
            BlockPos mPos;
            BaseCircuitComponent *mRawComponentPtr;
        };

        /*
         *Core data structure
         */
        struct TCircuitSceneGraph {
            std::unordered_map<BlockPos, std::unique_ptr<BaseCircuitComponent>> mAllComponents;
            std::vector<ComponentItem> mActiveComponents;
            std::unordered_map<BlockPos, std::vector<ComponentItem>> mActiveComponentsPerChunk;
            std::unordered_map<BlockPos, std::vector<ComponentItem>> mPowerAssociationMap;
        };

        static_assert(sizeof(ComponentItem) == 32);

        class TCircuitTrackingInfo {
           public:
            struct TEntry {
                BaseCircuitComponent *mComponent = nullptr;     // 0 - 8
                BlockPos mPos;                                  // 9 ~20
                trapdoor::TFACING mDirection = TFACING::NEG_Y;  // 21 ~ ?
                uint64_t type{};
            };

            static_assert(sizeof(TEntry) == 0x20);
            TEntry mCurrent{};
            TEntry mPower{};
            TEntry mNearest{};
            TEntry m2ndNearest{};
            bool mDirectlyPowered = true;
            int mData = 0;
            int mDampening = 0;
        };
        // 红石线

        std::string compTypeToStr(uint64_t type) {
            auto color = trapdoor::TextBuilder::WHITE;
            std::string str = "[unknown]";
            if (type == 0x100000) {
                color = trapdoor::TB::RED;
                str = "[Redstone Wine]";
            } else if (type == 0x80000) {
                color = trapdoor::TB::DARK_RED;
                str = "[Power Block]";
            } else if (type == 0x200000) {
                color = trapdoor::TB::AQUA;
                str = "[Capacitor]";
            } else if (type == 0x20000) {
                color = trapdoor::TB::GRAY;
                str = "[Consumer]";
            }
            trapdoor::TextBuilder builder;
            builder.sTextF(color, "%s", str.c_str());
            return builder.get();
        }

        std::string buildTrackMsg(TCircuitTrackingInfo *info, int *damping, bool *dirPow) {
            // 调用的时候near才是当前的
            // 2nd是上一个
            // cur是下一个

            auto &cur = info->mCurrent;
            auto &prev = info->mNearest;
            auto p = cur.mPos;

            auto color = *dirPow ? PCOLOR::TEAL : PCOLOR::COCOA;
            trapdoor::spawnNumParticle({(float)p.x + 0.5f, (float)p.y + 1.2f, (float)p.z + 0.5f},
                                       *damping, color, 0);
            return fmt::format(" -{}  -- {} --> {} ({},{},{}) IDamp:{} / damp: {} dp: {}",
                               compTypeToStr(prev.type), trapdoor::facingToString(prev.mDirection),
                               compTypeToStr(cur.type), p.x, p.y, p.z, info->mDampening, *damping,
                               *dirPow);
        }

    }  // namespace

    ActionResult displayRedstoneCompInfo(Dimension *d, const BlockPos &pos,
                                         const std::string &type) {
        if (!d) return ErrorDimension();
        if (pos == BlockPos::MAX) return ErrorPosition();

        auto &cs = d->getCircuitSystem();
        auto &graph = getCircuitSceneGraph(&cs);
        auto *g = reinterpret_cast<TCircuitSceneGraph *>(&graph);
        // 获取红石组件
        if (type == "chunk") {
            auto chunkPos = TBlockPos(pos.x, pos.y, pos.z).toChunkPos();
            BlockPos cp{chunkPos.x * 16, 0, chunkPos.z * 16};
            auto iter = g->mActiveComponentsPerChunk.find(cp);
            if (iter != g->mActiveComponentsPerChunk.end()) {
                for (auto &l : iter->second) {
                    auto lPos = l.mPos;
                    trapdoor::shortHighlightBlock({lPos.x, lPos.y, lPos.z}, PCOLOR::VATBLUE,
                                                  d->getDimensionId());
                }
            }

            return {"", true};
        }

        auto comp = graph.getBaseComponent(pos);
        if (!comp) {
            return {"Not a redstone component", false};
        }

        if (type == "signal") {
            TextBuilder builder;
            builder.text("Signal: ").num(comp->getStrength()).text("\n");
            auto &list = dAccess<std::vector<ComponentItem>, 8>(comp);
            for (auto &source : list) {
                auto p = source.mPos;
                builder.textF(" - Pos: [%s] Damp: %d Dp: %d Signal: %d\n", p.toString().c_str(),
                              source.mDampening, source.mDirectlyPowered,
                              source.mComponent->getStrength());
                auto sigColor = PCOLOR::RED;
                auto dampingColor = PCOLOR::DARK;
                auto sigPos = TVec3(p.x, p.y, p.z) + TVec3(0.5f, 1.9f, 0.5f);
                auto dampingPos = TVec3(p.x, p.y, p.z) + TVec3(0.5f, 1.3f, 0.5f);
                spawnNumParticle(sigPos, source.mComponent->getStrength(), sigColor,
                                 (int)d->getDimensionId());
                spawnNumParticle(dampingPos, source.mDampening, dampingColor,
                                 (int)d->getDimensionId());
            }
            return {builder.get(), true};
        }

        //        if (type == "torch") {
        //            return {"", true};
        //        }

        if (type != "conn") return {"", true};
        // link
        // 高亮自身
        trapdoor::shortHighlightBlock({pos.x, pos.y, pos.z}, PCOLOR::GREEN, d->getDimensionId());
        // 高亮被自身激活的原件
        auto it = g->mPowerAssociationMap.find(pos);
        if (it != g->mPowerAssociationMap.end()) {
            for (auto &c : it->second) {
                auto p = c.mPos;
                trapdoor::shortHighlightBlock({p.x, p.y, p.z}, PCOLOR::YELLOW, d->getDimensionId());
            }
        }

        // 高亮可以激活自身的原件
        auto &list = dAccess<std::vector<ComponentItem>, 8>(comp);
        for (auto &source : list) {
            auto p = source.mPos;
            trapdoor::shortHighlightBlock({p.x, p.y, p.z}, PCOLOR::RED, d->getDimensionId());
        }
        return {"", true};
    }
}  // namespace trapdoor

/*------------------------下面是构建电力相关的hook，由于会影响一定的性能因此这里先注释了---------------------------*/

// THook(void,
//       "?findRelationships@CircuitSceneGraph@@AEAAXAEBVBlockPos@@PEAVBaseCircuitComponent@@"
//       "PEAVBlockSource@@@Z",
//       void *graph, BlockPos const &pos, BaseCircuitComponent *comp, class BlockSource *bs) {
//     trapdoor::TextBuilder builder;
//     builder.sTextF(trapdoor::TB::BOLD | trapdoor::TB::GREEN, "[%s] ", pos.toString().c_str())
//         .text("Start a connection search\n");
//     trapdoor::shortHighlightBlock({pos.x, pos.y, pos.z}, trapdoor::PCOLOR::GREEN, 0);
//     // trapdoor::mod().getEventTriggerMgr().broadcastMessage(trapdoor::BuildConnection,
//     // builder.get());
//     trapdoor::broadcastMessage(builder.get());
//     original(graph, pos, comp, bs);
// }
//
// THook(bool,
//       "?addSource@TransporterComponent@@UEAA_NAEAVCircuitSceneGraph@@AEBVCircuitTrackingInfo@@"
//       "AEAHAEA_N@Z",
//       BaseCircuitComponent *self, CircuitSceneGraph *graph, trapdoor::TCircuitTrackingInfo *info,
//       int *damping, bool *directPowered) {
//     auto res = original(self, graph, info, damping, directPowered);
//     if (res) {
//         trapdoor::broadcastMessage(trapdoor::buildTrackMsg(info, damping, directPowered));
//     }
//
//     return res;
// }
//
//// 比较器
// THook(bool,
//       "?addSource@ComparatorCapacitor@@UEAA_NAEAVCircuitSceneGraph@@AEBVCircuitTrackingInfo@@"
//       "AEAHAEA_N@Z",
//       void *self, CircuitSceneGraph *graph, trapdoor::TCircuitTrackingInfo *info, int *damping,
//       bool *directPowered) {
//     auto res = original(self, graph, info, damping, directPowered);
//     if (res) {
//         trapdoor::broadcastMessage(trapdoor::buildTrackMsg(info, damping, directPowered));
//     }
//     return res;
// }
//
//// 消费者
//
// THook(bool,
//      "?addSource@ConsumerComponent@@UEAA_NAEAVCircuitSceneGraph@@AEBVCircuitTrackingInfo@@"
//      "AEAHAEA_"
//      "N@Z",
//      void *self, CircuitSceneGraph *graph, trapdoor::TCircuitTrackingInfo *info, int *damping,
//      bool *directPowered) {
//    auto res = original(self, graph, info, damping, directPowered);
//    if (res) {
//        trapdoor::broadcastMessage(trapdoor::buildTrackMsg(info, damping, directPowered));
//    }
//    return res;
//}

/*
 * void CircuitSceneGraph::scheduleRelationshipUpdate(class BlockPos const &, class
BaseCircuitComponent *)
[25395104]?scheduleRelationshipUpdate@CircuitSceneGraph@@AEAAXAEBVBlockPos@@PEAVBaseCircuitComponent@@@Z
 */

// THook(void,
//       "?scheduleRelationshipUpdate@CircuitSceneGraph@@AEAAXAEBVBlockPos@@PEAVBaseCircuitComponent@@"
//       "@Z",
//       void *self, const BlockPos &pos, void *comp) {
//     trapdoor::shortHighlightBlock({pos.x, pos.y, pos.z}, trapdoor::PINK, 0);
//     original(self, pos, comp);
// }