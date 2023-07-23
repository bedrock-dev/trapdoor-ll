#include "BlockRotateHelper.h"

#include <mc/BedrockBlocks.hpp>
#include <mc/Block.hpp>
#include <mc/BlockLegacy.hpp>
#include <mc/BlockSource.hpp>
#include <mc/ByteTag.hpp>
#include <mc/CommandUtils.hpp>
#include <mc/IntTag.hpp>
#include <mc/StringTag.hpp>
#include <magic_enum/magic_enum.hpp>
#include <regex>

#include "CommandHelper.h"
#include "TrapdoorMod.h"
#include "Utils.h"

namespace trapdoor {
    namespace {
        struct RotateRule {
            std::string namePatterns;
            std::function<int(int, unsigned char)> func;
        };

        std::vector<RotateRule> &globalRotateRules() {
            static std::vector<RotateRule> rules;
            return rules;
        }

    }  // namespace

#define ADD_RULE(pattern, func) \
    globalRotateRules().push_back({#pattern, [](int v, unsigned char face) { return func; }})

    void initRotateBlockHelper() {
        ADD_RULE(stonecutter_block, v < 4 ? 4 : v);
        ADD_RULE(bell, (v + 1) % 16);
        ADD_RULE(torch, (v + 1) % 5);
        ADD_RULE(grindstone, (v + 1) % 12);
//        ADD_RULE(_rail, (v % 8 + 1) % 6 + (v / 8) * 8);
        ADD_RULE(lever, (v % 8 + 1) % 6 + (v / 8) * 8);
        ADD_RULE(rot, (v % 8 + 1) % 6 + (v / 8) * 8);
//        ADD_RULE(rail, (v + 1) % 10);
//        ADD_RULE(powered, (v % 4 + 1) % 4 + (v / 4) * 4);
        ADD_RULE(anvil, (v % 4 + 1) % 4 + (v / 4) * 4);
        ADD_RULE(lectern, (v % 4 + 1) % 4 + (v / 4) * 4);
        ADD_RULE(_door, (v % 4 + 1) % 4 + (v / 4) * 4);
        ADD_RULE(loom, (v % 4 + 1) % 4 + (v / 4) * 4);
        ADD_RULE(trapdoor, (v % 8 + 1) % 8 + (v / 8) * 8);
        ADD_RULE(hopper, (v % 8 + 1) % 6 + (v / 8) * 8);
    }

    Block *tryGetRotatedBlock(Block *block, BlockSource *blockSource, BlockPos const &pos,
                              const Vec3 &clickPos, unsigned char face) {
        auto rawTypeName = block->getTypeName();
        auto variant = block->getVariant();
        auto typeName = trapdoor::rmmc(rawTypeName);
        auto it =
            std::find_if(globalRotateRules().begin(), globalRotateRules().end(),
                         [&typeName](const RotateRule &rule) {
                             return std::regex_search(typeName, std::regex(rule.namePatterns));
                         });
        if (it != globalRotateRules().end()) {
            return Block::create(rawTypeName, it->func(variant, face));
        }
        auto blockNbt = block->getNbt();
        if (!blockNbt->contains("states")) {
            return block;
        }
        auto *statesNbt = blockNbt->getCompound("states");
        bool hasRule = false;
        if (statesNbt->contains("ground_sign_direction") &&
            (!statesNbt->contains("attached_bit") || statesNbt->getByte("attached_bit") == 1)) {
            auto direction = statesNbt->getInt("ground_sign_direction");
            direction = (direction + 1) % 16;
            statesNbt->putInt("ground_sign_direction", direction);
            hasRule = true;
        } else if (statesNbt->contains("pillar_axis")) {
            char axis = statesNbt->getString("pillar_axis")[0];
            axis -= 'x';
            axis = (axis + 1) % 3;
            axis += 'x';
            statesNbt->putString("pillar_axis", std::string(1, axis));
            hasRule = true;
        } else if (statesNbt->contains("upside_down_bit") && face < 2) {
            auto bit = statesNbt->getByte("upside_down_bit");
            bit = 1 - bit;
            statesNbt->putByte("upside_down_bit", bit);
            hasRule = true;
        } else if (statesNbt->contains("weirdo_direction") && face > 1) {
            auto direction = statesNbt->getInt("weirdo_direction");
            direction = (direction + 1) % 4;
            statesNbt->putInt("weirdo_direction", direction);
            hasRule = true;
        } else if (statesNbt->contains("top_slot_bit")) {
            auto bit = statesNbt->getByte("top_slot_bit");
            bit = 1 - bit;
            statesNbt->putByte("top_slot_bit", bit);
            hasRule = true;
        } else if (statesNbt->contains("facing_direction")) {
            auto direction = statesNbt->getInt("facing_direction");
            if (typeName.find("_sign") != string::npos) {
                direction = (direction + 1) % 6;
            } else {
                Vec3 mClickPos = (clickPos - pos.toVec3()) - 0.5;
                if (abs(mClickPos.x) + abs(mClickPos.y) + abs(mClickPos.z) < 0.75) {
                    auto mFace = face;
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
            }
            statesNbt->putInt("facing_direction", direction);
            hasRule = true;
        } else if (statesNbt->contains("direction")) {
            auto direction = statesNbt->getInt("direction");
            Vec3 mClickPos = (clickPos - pos.toVec3()) - 0.5;
            if (abs(mClickPos.x) + abs(mClickPos.y) + abs(mClickPos.z) < 0.75 && face > 1) {
                int mFace = std::array{2, 0, 1, 3}[face - 2];
                if (direction == mFace) {
                    direction = std::array{0, 2, 3, 1}[face - 2];
                } else {
                    direction = mFace;
                }
            } else {
                switch (face / 2) {
                    case 0:
                        if (abs(mClickPos.x) + mClickPos.z <= 0) {
                            direction = 2;
                        } else if (abs(mClickPos.x) - mClickPos.z <= 0) {
                            direction = 0;
                        } else if (abs(mClickPos.z) + mClickPos.x <= 0) {
                            direction = 1;
                        } else if (abs(mClickPos.z) - mClickPos.x <= 0) {
                            direction = 3;
                        }
                        break;
                    case 1:
                        if (mClickPos.x <= 0) {
                            direction = 1;
                        } else if (mClickPos.x >= 0) {
                            direction = 3;
                        }
                        break;
                    case 2:
                        if (mClickPos.z <= 0) {
                            direction = 2;
                        } else if (mClickPos.z >= 0) {
                            direction = 0;
                        }
                        break;
                    default:
                        break;
                }
            }
            statesNbt->putInt("direction", direction);
            hasRule = true;
        } else if (statesNbt->contains("minecraft:facing_direction")) { // 目前是观察者，也许有其它方块也是这个？
            auto directionStr = statesNbt->getString("minecraft:facing_direction");
            // Nbt里minecraft:facing_direction的值首字母是小写的，FaceID首字母大写，所以转换一下
            directionStr[0] = std::toupper(directionStr[0]);
            // string -> enum
            FaceID directionEnum = magic_enum::enum_cast<FaceID>(directionStr).value();
            auto direction = (int)directionEnum;
            Vec3 mClickPos = (clickPos - pos.toVec3()) - 0.5;
            if (abs(mClickPos.x) + abs(mClickPos.y) + abs(mClickPos.z) < 0.75) {
                auto mFace = face;
                mFace = (mFace > 1 && (typeName == "observer" ))
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
                direction = (direction > 1 && (typeName == "observer"))
                                ? (direction / 2) * 2 + (direction + 1) % 2
                                : direction;
            }
            // enum -> string
            std::string result = std::string{magic_enum::enum_name(FaceID(direction))};
            // 转回小写
            result[0] = std::tolower(result[0]);
            statesNbt->putString("minecraft:facing_direction", result);
            hasRule = true;
        } else if (statesNbt->contains("rail_direction")) { // 这个是铁轨的
            auto direction = statesNbt->getInt("rail_direction");
            if (typeName == "rail") {
                // 普通铁轨
                direction = (direction + 1) % 10;
            } else {
                // 动力、激活、探测铁轨
                direction = (direction % 8 + 1) % 6 + (direction / 8) * 8;
            }
            statesNbt->putInt("rail_direction",direction);
            hasRule = true;
        }
        if (hasRule) {
            return Block::create(blockNbt.get());
        }
        return block;
    }

    bool rotateBlock(Player *player, BlockSource *bs, BlockInstance *bi, const Vec3 &clickPos,
                     unsigned char face) {
        if (!trapdoor::mod().getConfig().getGlobalFunctionConfig().blockRotate) return true;
        if (!player || !trapdoor::mod().getUserConfig().blockrotate(player->getRealName()))
            return true;

        if (!bi || bi->isNull()) return true;
        auto *originalBlock = bi->getBlock();
        // auto *block = originalBlock;
        auto rawTypeName = originalBlock->getTypeName();
        auto pos = bi->getPosition();
        auto typeName = trapdoor::rmmc(rawTypeName);
        // int i = 0;
        // do {
        //     block = tryGetRotatedBlock(block, bs, pos, clickPos, face);
        //     i += 1;
        // } while (block != originalBlock && (!block->mayPlace(*bs, pos)) && i < 32);

        auto *block = tryGetRotatedBlock(originalBlock, bs, pos, clickPos, face);

        if (block != originalBlock) {
            if (typeName == "piston" || typeName == "sticky_piston" || !bi->hasBlockEntity()) {
                CommandUtils::clearBlockEntityContents(*bs, pos);
                auto *exBlock = &bs->getExtraBlock(pos);
                bs->setExtraBlock(pos, *BedrockBlocks::mAir, 2);
                bs->setBlock(pos, *BedrockBlocks::mAir, 2, nullptr, nullptr);
                bs->setExtraBlock(pos, *exBlock, 2);
            }
            bs->setBlock(pos, *block, 2, nullptr, nullptr);
        } else {
            trapdoor::logger().debug("rotateBlock: no rule for {}", typeName);
            return true;
        }
        return false;
    }

}  // namespace trapdoor
