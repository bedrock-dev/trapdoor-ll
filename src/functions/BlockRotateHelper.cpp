#include "BlockRotateHelper.h"

#include <MC/Block.hpp>
#include <MC/BlockLegacy.hpp>
#include <MC/BlockSource.hpp>
#include <MC/ByteTag.hpp>
#include <MC/IntTag.hpp>
#include <MC/StringTag.hpp>
#include <regex>

#include "TrapdoorMod.h"
#include "Utils.h"
namespace trapdoor {
    namespace {
        struct RotateRule {
            std::string namePatterns;
            std::function<int(int, Vec3 const &, unsigned char)> func;
        };

        std::vector<RotateRule> &globalRotateRules() {
            static std::vector<RotateRule> rules;
            return rules;
        }
    }  // namespace

#define ADD_RULE(pattern, func)    \
    globalRotateRules().push_back( \
        {#pattern, [](int v, Vec3 const &clickPos, unsigned char face) { return func; }})

    void initRotateBlockHelper() {
        ADD_RULE(stonecutter_block, v < 4 ? 4 : v);
        ADD_RULE(bell, (v + 1) % 16);
        ADD_RULE(torch, (v + 1) % 5);
        ADD_RULE(grindstone, (v + 1) % 12);
        ADD_RULE(_rail, (v % 8 + 1) % 6 + (v / 8) * 8);
        ADD_RULE(lever, (v % 8 + 1) % 6 + (v / 8) * 8);
        ADD_RULE(rot, (v % 8 + 1) % 6 + (v / 8) * 8);
        ADD_RULE(barrel, (v % 8 + 1) % 6 + (v / 8) * 8);
        ADD_RULE(rail, (v + 1) % 10);
        ADD_RULE(powered, (v % 4 + 1) % 4 + (v / 4) * 4);
        ADD_RULE(anvil, (v % 4 + 1) % 4 + (v / 4) * 4);
        ADD_RULE(lectern, (v % 4 + 1) % 4 + (v / 4) * 4);
        ADD_RULE(_door, (v % 4 + 1) % 4 + (v / 4) * 4);
        ADD_RULE(loom, (v % 4 + 1) % 4 + (v / 4) * 4);
        ADD_RULE(trapdoor, (v % 8 + 1) % 8 + (v / 8) * 8);
        ADD_RULE(hopper, (v % 8 + 1) % 6 + (v / 8) * 8);
    }

    bool rotateBlock(BlockSource *bs, BlockInstance *bi, const Vec3 &clickPos, unsigned char face) {
        if (!bi || bi->isNull()) return true;

        auto block = bi->getBlock();
        auto variant = dAccess<unsigned short, 8>(block);
        auto rawTypeName = block->getTypeName();
        auto pos = bi->getPosition();
        auto typeName = trapdoor::rmmc(rawTypeName);

        trapdoor::logger().debug("Block: {}, Data: {}, Position: {}, Click: {} Face: {}", typeName,
                                 variant, pos.toString(), clickPos.toString(),
                                 static_cast<int>(face));

        auto it =
            std::find_if(globalRotateRules().begin(), globalRotateRules().end(),
                         [&typeName](const RotateRule &rule) {
                             return std::regex_search(typeName, std::regex(rule.namePatterns));
                         });
        if (it != globalRotateRules().end()) {
            auto newVariant = it->func(variant, clickPos, face);
            bs->setBlock(pos, *Block::create(rawTypeName, newVariant), 3, nullptr, nullptr);
        } else {
            auto blockNbt = block->getNbt();
            auto *statesNbt = blockNbt->operator[]("states")->asCompoundTag();
            auto *states = &statesNbt->value();
            bool hasRule = false;
            if (states->find("pillar_axis") != states->end()) {
                auto *tag = statesNbt->operator[]("pillar_axis")->asStringTag();
                char axis = tag->get()[0];
                axis -= 'x';
                axis = (axis + 1) % 3;
                axis += 'x';
                statesNbt->putString("pillar_axis", std::string(1, axis));
                hasRule = true;
            } else if (states->find("upside_down_bit") != states->end() && face < 2) {
                auto *tag = statesNbt->operator[]("upside_down_bit")->asByteTag();
                auto bit = tag->get();
                bit = 1 - bit;
                statesNbt->putByte("upside_down_bit", bit);
                hasRule = true;
            } else if (states->find("top_slot_bit") != states->end()) {
                auto *tag = statesNbt->operator[]("top_slot_bit")->asByteTag();
                auto bit = tag->get();
                bit = 1 - bit;
                statesNbt->putByte("top_slot_bit", bit);
                hasRule = true;
            } else if (states->find("facing_direction") != states->end()) {
                auto *tag = statesNbt->operator[]("facing_direction")->asIntTag();
                auto direction = tag->get();
                auto mFace = face > 1 ? (face / 2) * 2 + (face + 1) % 2 : face;
                if (direction == mFace) {
                    direction = (mFace / 2) * 2 + (mFace + 1) % 2;
                } else {
                    direction = mFace;
                }
                statesNbt->putInt("facing_direction", direction);
                hasRule = true;
            }
            if (hasRule) {
                bs->setBlock(pos, *Block::create(blockNbt.get()), 3, nullptr, nullptr);
            } else {
                trapdoor::logger().debug("rotateBlock: no rule for {}", typeName);
            }
            return true;
        }

        return false;
    }
}  // namespace trapdoor
