//
// Created by xhy on 2022/6/29.
//

#include "InventoryTool.h"

#include <MC/Block.hpp>
#include <MC/Inventory.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Material.hpp>
#include <cstdio>

#include "Config.h"
#include "TrapdoorMod.h"
#include "Utils.h"
namespace tr {
    namespace {
        enum ToolType {
            PICK_AXE = 0,  // 镐子
            AXE = 1,       // 斧头
            SHOVEL = 2,    // 铲子
            SWORD = 3,     // 剑
            HOE = 4,       // 锄头
            SHEARS = 5,    // 剪刀
            EMPTY = 6      // 空手
        };

        struct ToolTypeInfo {
            int quality = -1;
            int slot = 0;
            ToolType type = EMPTY;
            int remainDamage = 0;
            friend bool operator<(const ToolTypeInfo &lhs, const ToolTypeInfo &rhs) {
                if (lhs.quality < rhs.quality) return true;
                if (rhs.quality < lhs.quality) return false;
                return lhs.remainDamage > rhs.remainDamage;
            }
        };

        std::map<int, std::vector<ToolType>> &getToolMap() {
            static std::map<int, std::vector<ToolType>> map = {
                {0x1, {SHOVEL}},            // 泥土
                {0x2, {AXE}},               // 木头
                {0x3, {PICK_AXE}},          // 石头
                {0x4, {PICK_AXE}},          // 金属
                {0x7, {SHEARS, HOE, AXE}},  // 树叶
                {0x8, {AXE}},               // 植物
                {0x9, {AXE}},               // 可替换植物
                {0xa, {PICK_AXE}},
                {0xb, {HOE}},             // 海绵
                {0xc, {SHEARS}},          // 羊毛
                {0xd, {AXE}},             // 床
                {0xe, {SHOVEL}},          // 沙子
                {0xf, {SHOVEL}},          // Decoration
                {0x10, {PICK_AXE}},       // 玻璃 头颅
                {0x11, {PICK_AXE}},       // Explosive
                {0x12, {PICK_AXE}},       // 冰
                {0x13, {PICK_AXE}},       // PackedIce
                {0x14, {PICK_AXE}},       // 浮冰
                {0x15, {SHOVEL}},         // 顶层雪
                {0x16, {SHOVEL}},         // 雪
                {0x17, {PICK_AXE}},       //
                {0x18, {HOE}},            // Vegetable
                {0x19, {PICK_AXE}},       // 仙人掌
                {0x1a, {SHOVEL}},         // 粘土 怪物蛋
                {0x1b, {AXE}},            // 南瓜
                {0x1c, {EMPTY}},          // 红实线
                {0x1d, {AXE}},            // 地毯
                {0x1e, {SWORD, SHEARS}},  // 蜘蛛网
                {0x1f, {EMPTY}},          // 史莱姆
                {0x20, {PICK_AXE}},       // 活塞
                {0x21, {PICK_AXE}},       // 红石灯
                {0x22, {PICK_AXE}},       // Deny
                {0x23, {PICK_AXE}},       // 活塞
                {0x24, {PICK_AXE}},       // StoneDecoration
                {0x25, {PICK_AXE}},       // 气泡
                {0x26, {HOE}},            // 下界疣块
                {0x27, {PICK_AXE}},       // 屏障
                {0x28, {PICK_AXE}},       // DecorationFlammable
                {0x29, {PICK_AXE}},       // SurfaceTypeTotal
                {0x2a, {PICK_AXE}}        // Any_1
            };
            return map;
        }
        std::vector<ToolType> getToolType(const Block *block) {
            auto &map = getToolMap();
            auto &m = block->getMaterial();
            for (auto &kv : map) {
                if (m.isType((MaterialType)kv.first)) {
                    tr::logger().debug("Material is 0x{:x}", kv.first);
                    if (kv.second.empty()) {
                        tr::logger().warn("Block {:x} has material {}, but no tools is assigned",
                                          block->getTypeName(), kv.first);
                    }
                    return kv.second;
                }
            }
            tr::logger().warn("No Prop Material for block {}", block->getTypeName());
            return {};
        }

        ToolTypeInfo tryCollectToolInfo(const ItemStack *item) {
            ToolTypeInfo info;
            auto typeName = tr::rmmc(item->getTypeName());
            static std::array<std::pair<std::string, ToolType>, 6> array = {
                std::make_pair("_pickaxe", PICK_AXE), std::make_pair("_axe", AXE),
                std::make_pair("_shovel", SHOVEL),    std::make_pair("_sword", SWORD),
                std::make_pair("_hoe", HOE),          std::make_pair("shears", SHEARS),
            };
            static std::map<std::string, int> toolQualityMap = {{"wooden", 0},    {"stone", 1},
                                                                {"iron", 2},      {"diamond", 3},
                                                                {"netherite", 4}, {"golden", 4}};
            for (auto &t : array) {
                auto p = typeName.find(t.first);
                if (p != std::string::npos) {
                    info.type = t.second;
                    if (info.type == SHEARS) {
                        info.quality = 0;
                        info.remainDamage = item->getMaxDamage() - item->getDamageValue();
                        return info;
                    }

                    auto q = typeName.substr(0, p);
                    auto iter = toolQualityMap.find(q);
                    if (iter != toolQualityMap.end()) {
                        info.quality = iter->second;
                        info.remainDamage = item->getMaxDamage() - item->getDamageValue();
                        return info;
                    }
                }
            }
            info.quality = -1;
            return info;
        }

        int searchBestToolInInv(Player *p, const std::vector<ToolType> &propTools,
                                int minRemainDamage) {
            std::map<ToolType, std::set<ToolTypeInfo>> tools;
            auto &inv = p->getInventory();
            // 搜集所有合适的工具
            int sz = inv.getSize();

            for (int i = 0; i < sz; i++) {
                auto *item = inv.getSlot(i);
                if (item && item->getCount() != 0) {
                    auto toolInfo = tryCollectToolInfo(item);
                    if (toolInfo.quality != -1 && toolInfo.remainDamage > minRemainDamage) {
                        toolInfo.slot = i;
                        tr::logger().debug("find {} in slot {} with remain damage {}",
                                           item->getTypeName(), i, toolInfo.remainDamage);
                        tools[toolInfo.type].insert(toolInfo);
                    }
                }
            }

            // 选择最合适的
            for (auto &type : propTools) {
                tr::logger().debug("type: {}", type);
                auto iter = tools.find(type);
                if (iter == tools.end() || iter->second.empty()) continue;
                tr::logger().debug("select tool in slot {}", iter->second.rbegin()->slot);
                return iter->second.rbegin()->slot;
            }
            return -1;
        }
    }  // namespace

    void onStartDestroyBlock(Player *player, const BlockInstance &instance) {
        if (!tr::mod().getConfig().getTweakConfig().autoSelectTool) {
            return;
        }
        auto *ins = const_cast<BlockInstance *>(&instance);
        if (!player || !ins || ins->isNull()) return;
        auto propTools = getToolType(ins->getBlock());
        auto targetSlot = searchBestToolInInv(player, propTools, 1);
        if (targetSlot == -1) {
            tr::logger().debug("No tools found\n");
        } else {
            if (targetSlot <= 8) {
                player->setSelectedSlot(targetSlot);
                return;
            }

            auto &inv = player->getInventory();
            auto selSlot = player->getSelectedItemSlot();
            auto select = inv.getItem(selSlot).clone();
            auto target = inv.getItem(targetSlot).clone();
            inv.removeItem(player->getSelectedItemSlot(), 64);
            inv.removeItem(targetSlot, 64);
            inv.setItem(player->getSelectedItemSlot(), target);
            inv.setItem(targetSlot, select);
        }
    }
}  // namespace tr
