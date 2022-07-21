#include "BlockRotateHelper.h"

#include <MC/BedrockBlocks.hpp>
#include <MC/Block.hpp>
#include <MC/BlockLegacy.hpp>
#include <MC/BlockSource.hpp>
#include <MC/ByteTag.hpp>
#include <MC/CommandUtils.hpp>
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
            if (!bi->hasContainer()) {
                CommandUtils::clearBlockEntityContents(*bs, pos);
                auto *exBlock = &bs->getExtraBlock(pos);
                bs->setExtraBlock(pos, *BedrockBlocks::mAir, 18);
                bs->setBlock(pos, *BedrockBlocks::mAir, 2, nullptr, nullptr);
                bs->setExtraBlock(pos, *exBlock, 18);
            }
            bs->setBlock(pos, *Block::create(rawTypeName, newVariant), 2, nullptr, nullptr);
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
            } else if (states->find("weirdo_direction") != states->end() && face < 2) {
                auto *tag = statesNbt->operator[]("weirdo_direction")->asByteTag();
                auto direction = tag->get();
                direction = (direction + 1) % 4;
                statesNbt->putByte("weirdo_direction", direction);
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
                auto mFace = face;
                Vec3 mClickPos = (clickPos - pos.toVec3()) - 0.5;
                if (abs(mClickPos.x) + abs(mClickPos.y) + abs(mClickPos.z) < 0.75) {
                    mFace = (mFace > 1 && (typeName == "piston" || typeName == "observer" ||
                                           typeName == "sticky_piston"))
                                ? (mFace / 2) * 2 + (mFace + 1) % 2
                                : mFace;
                    if (direction == mFace) {
                        direction = (mFace / 2) * 2 + (mFace + 1) % 2;
                    } else {
                        direction = mFace;
                    }
                } else {
                    switch (face / 2) {
                        case 0:
                            if (abs(mClickPos.x) + mClickPos.z <= 0) {
                                direction = 2;
                            } else if (abs(mClickPos.x) - mClickPos.z <= 0) {
                                direction = 3;
                            } else if (abs(mClickPos.z) + mClickPos.x <= 0) {
                                direction = 4;
                            } else if (abs(mClickPos.z) - mClickPos.x <= 0) {
                                direction = 5;
                            }
                            break;
                        case 1:
                            if (abs(mClickPos.x) + mClickPos.y <= 0) {
                                direction = 0;
                            } else if (abs(mClickPos.x) - mClickPos.y <= 0) {
                                direction = 1;
                            } else if (abs(mClickPos.y) + mClickPos.x <= 0) {
                                direction = 4;
                            } else if (abs(mClickPos.y) - mClickPos.x <= 0) {
                                direction = 5;
                            }
                            break;
                        case 2:
                            if (abs(mClickPos.z) + mClickPos.y <= 0) {
                                direction = 0;
                            } else if (abs(mClickPos.z) - mClickPos.y <= 0) {
                                direction = 1;
                            } else if (abs(mClickPos.y) + mClickPos.z <= 0) {
                                direction = 2;
                            } else if (abs(mClickPos.y) - mClickPos.z <= 0) {
                                direction = 3;
                            }
                            break;
                    }
                    direction = (direction > 1 && (typeName == "piston" || typeName == "observer" ||
                                                   typeName == "sticky_piston"))
                                    ? (direction / 2) * 2 + (direction + 1) % 2
                                    : direction;
                }
                statesNbt->putInt("facing_direction", direction);
                hasRule = true;
            }
            if (hasRule) {
                if (!bi->hasContainer()) {
                    CommandUtils::clearBlockEntityContents(*bs, pos);
                    auto *exBlock = &bs->getExtraBlock(pos);
                    bs->setExtraBlock(pos, *BedrockBlocks::mAir, 18);
                    bs->setBlock(pos, *BedrockBlocks::mAir, 2, nullptr, nullptr);
                    bs->setExtraBlock(pos, *exBlock, 18);
                }
                bs->setBlock(pos, *Block::create(blockNbt.get()), 2, nullptr, nullptr);
            } else {
                trapdoor::logger().debug("rotateBlock: no rule for {}", typeName);
                return true;
            }
            return true;
        }

        return false;
    }
}  // namespace trapdoor
