//
// Created by xhy on 2022/6/29.
//

#include "AutomaticTool.h"

#include <MC/Block.hpp>
#include <MC/Inventory.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Material.hpp>

#include "TrapdoorMod.h"
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
            int level = -1;
            int slot = 0;
            ToolType type = EMPTY;
            int durable = 0;
            friend bool operator<(const ToolTypeInfo &lhs, const ToolTypeInfo &rhs) {
                if (lhs.level < rhs.level) return true;
                if (rhs.level < lhs.level) return false;
                return lhs.durable < rhs.durable;
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
                {0xa, {HOE}},               // 海绵
                {0xb, {}},                  // Cloth
                {0xc, {AXE}},               // 床
                {0xd, {}},
                {0xe, {SHOVEL}},     // 沙子
                {0xf, {SHOVEL}},     // Decoration
                {0x10, {PICK_AXE}},  // 玻璃
                {0x11, {}},          // Explosive
                {0x12, {PICK_AXE}},  // 冰
                {0x13, {PICK_AXE}},  // PackedIce
                {0x14, {SHOVEL}},    // 顶层雪
                {0x15, {SHOVEL}},    // 雪
                {0x16, {AXE}},       // 仙人掌
                {0x17, {PICK_AXE}},  // 粘土
                {0x18, {HOE}},       // Vegetable
                {0x19, {}},          // Portal_1
                {0x1a, {AXE}},       // 蛋糕
                {0x1b, {SWORD}},     // 蜘蛛网
                {0x1c, {EMPTY}},     // 红实线
                {0x1d, {AXE}},       // 地毯
                {0x1e, {AXE}},       // BuildableGlass
                {0x1f, {EMPTY}},     // 史莱姆
                {0x20, {PICK_AXE}},  // 活塞
                {0x21, {}},          // Allow
                {0x22, {}},          // Deny
                {0x23, {HOE}},       // 地狱疣
                {0x24, {}},          // StoneDecoration
                {0x25, {}},          // 气泡
                {0x26, {PICK_AXE}},  // 蛋
                {0x27, {}},          // 屏障
                {0x28, {}},          // DecorationFlammable
                {0x29, {}},          // SurfaceTypeTotal
                {0x2a, {}}           // Any_1
            };
            return map;
        }
        std::vector<ToolType> getToolType(const Material &m) {
            auto &map = getToolMap();
            for (auto &kv : map) {
                if (m.isType((MaterialType)kv.first)) {
                    tr::logger().debug("Material is {}", kv.first);
                    return kv.second;
                }
            }
            return {};
        }

        ToolTypeInfo itemStackToToolType(const ItemStack *item) {
            ToolTypeInfo info;
            info.level = 0;
            auto name = item->getTypeName();
            if (name.find("pickaxe") != std::string::npos) {
                info.type = PICK_AXE;
                return info;
            }

            if (name.find("axe") != std::string::npos) {
                info.type = AXE;
                return info;
            }

            if (name.find("shovel") != std::string::npos) {
                info.type = SHOVEL;
                return info;
            }
            if (name.find("sword") != std::string::npos) {
                info.type = SWORD;
                return info;
            }

            if (name.find("SHEAR") != std::string::npos) {
                info.type = SHEARS;
                return info;
            }
            info.level = -1;
            return info;
        }

        int searchBestToolInInv(Player *p, const std::vector<ToolType> &propTools, int minDurable) {
            std::map<ToolType, std::vector<ToolTypeInfo>> tools;
            auto &inv = p->getInventory();
            int sz = inv.getSize();
            for (int i = 0; i < sz; i++) {
                auto *item = inv.getSlot(i);
                if (item && item->getCount() != 0) {
                    auto toolInfo = itemStackToToolType(item);
                    if (toolInfo.level != -1) {
                        toolInfo.slot = i;
                        tr::logger().debug("find {} in slot {}", item->getTypeName(), i);
                        tools[toolInfo.type].push_back(toolInfo);
                    }
                }
            }

            for (auto &type : propTools) {
                tr::logger().debug("type: {}", type);
                auto iter = tools.find(type);
                if (iter == tools.end() || iter->second.empty()) continue;
                tr::logger().debug("select tool in slot {}", iter->second.front().slot);
                return iter->second.front().slot;
            }
            return -1;
        }
    }  // namespace

    bool onStartDestroyBlock(Player *player, const BlockInstance &instance) {
        auto *ins = const_cast<BlockInstance *>(&instance);
        if (!player || !ins || ins->isNull()) return true;
        auto propTools = getToolType(ins->getBlock()->getMaterial());
        tr::logger().debug("Proto tools: {}", propTools.size());
        auto slot = searchBestToolInInv(player, propTools, 0);

        if (slot == -1) {
            tr::logger().debug("No tools found\n");
        } else {
            // TODO
            player->setSelectedSlot(slot);
        }
        return true;
    }

}  // namespace tr
