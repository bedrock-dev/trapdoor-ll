#include "BlockRotateHelper.h"

#include <MC/Block.hpp>
#include <MC/BlockLegacy.hpp>
#include <MC/BlockSource.hpp>
#include <regex>

#include "TrapdoorMod.h"
#include "Utils.h"
namespace tr {

    namespace {
        struct RotateRule {
            std::string namePatterns;
            std::function<int(int)> func;
        };

        std::vector<RotateRule> &globalRotateRules() {
            static std::vector<RotateRule> rules;
            return rules;
        }
    }
#define ADD_RULE(pattern, func) \
    globalRotateRules().push_back({#pattern, [](int v) { return func; }})
        void initRotateBlockHelper() {
            ADD_RULE(slab, (v + 8) % 16);
            ADD_RULE(stonecutter_block, v < 4 ? 4 : v);
            ADD_RULE(bell, (v + 1) % 16);
            ADD_RULE(stair, (v + 4) % 8);
            ADD_RULE(torch, (v + 1) % 5);
            ADD_RULE(grindstone, (v + 1) % 12);
            ADD_RULE(_rail, (v % 8 + 1) % 6 + (v / 8) * 8);
            ADD_RULE(dropper, (v % 8 + 1) % 6 + (v / 8) * 8);
            ADD_RULE(dispenser, (v % 8 + 1) % 6 + (v / 8) * 8);
            ADD_RULE(observer, (v % 8 + 1) % 6 + (v / 8) * 8);
            ADD_RULE(piston, (v % 8 + 1) % 6 + (v / 8) * 8);
            ADD_RULE(lever, (v % 8 + 1) % 6 + (v / 8) * 8);
            ADD_RULE(button, (v % 8 + 1) % 6 + (v / 8) * 8);
            ADD_RULE(rot, (v % 8 + 1) % 6 + (v / 8) * 8);
            ADD_RULE(barrel, (v % 8 + 1) % 6 + (v / 8) * 8);
            ADD_RULE(glazed_terracotta, (v % 8 + 1) % 6 + (v / 8) * 8);
            ADD_RULE(rail, (v + 1) % 10);
            ADD_RULE(stripped, (v + 1) % 3);
            ADD_RULE(basalt, (v + 1) % 3);
            ADD_RULE(crimson_stem, (v + 1) % 3);
            ADD_RULE(warped_stem, (v + 1) % 3);
            ADD_RULE(powered, (v % 4 + 1) % 4 + (v / 4) * 4);
            ADD_RULE(anvil, (v % 4 + 1) % 4 + (v / 4) * 4);
            ADD_RULE(lectern, (v % 4 + 1) % 4 + (v / 4) * 4);
            ADD_RULE(_door, (v % 4 + 1) % 4 + (v / 4) * 4);
            ADD_RULE(loom, (v % 4 + 1) % 4 + (v / 4) * 4);
            ADD_RULE(trapdoor, (v % 8 + 1) % 8 + (v / 8) * 8);
            ADD_RULE(hopper, (v % 8 + 1) % 6 + (v / 8) * 8);
        }

    // namespace
    void rotateBlock(BlockSource &bs, const BlockPos &pos) {
        tr::logger().debug("rotateBlock: {} {} {} ", pos.x, pos.y, pos.z);
        auto &block = bs.getBlock(pos);
        auto variant = block.getVariant();
        auto typeName = tr::rmmc(block.getTypeName());
        auto it =
            std::find_if(globalRotateRules().begin(), globalRotateRules().end(),
                         [&typeName](const RotateRule &rule) {
                             return std::regex_search(typeName, std::regex(rule.namePatterns));
                         });
        if (it == globalRotateRules().end()) {
            tr::logger().debug("rotateBlock: no rule for {}", typeName);
            return;
        }
        auto newVariant = it->func(variant);
        auto *newBlock = block.getLegacyBlock().tryGetStateFromLegacyData(newVariant);
        bs.setBlock(pos.x, pos.y, pos.z, *newBlock, 3, nullptr);

    }
}  // namespace tr
