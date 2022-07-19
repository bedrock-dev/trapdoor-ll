//
// Created by xhy on 2022/7/19.
//

#include "DistanceHelper.h"

#include <unordered_map>

#include "Msg.h"
namespace trapdoor {

    namespace {

        std::unordered_map<std::string, BlockPos> &positionCache() {
            static std::unordered_map<std::string, BlockPos> cache;
            return cache;
        }

        std::string getDistanceInfo(const BlockPos &p1, const BlockPos &p2) {
            TextBuilder builder;
            /* [x,y,z] ~ [x,y,z]
             * Delta:       ~,~,~
             * Distance:    12,122
             * Manhattan:   123, 12
             */
            auto d = p2 - p1;
            builder.sTextF(TB::BOLD | TB::WHITE, "   [%s]", p2.toString().c_str())
                .text(" to ")
                .sTextF(TB::BOLD | TB::WHITE, "[%s]\n", p1.toString().c_str())
                .sText(TB::GRAY, " - ")
                .text("Dx / dy / dz: ")
                .num(d.x)
                .text(" / ")
                .num(d.y)
                .text(" / ")
                .num(d.z)
                .text("\n");
            builder.sText(TB::GRAY, " - ")
                .text("Distance: ")
                .num(sqrt(d.x * d.x + d.y * d.y + d.z * d.z))
                .text(" / ")
                .num(sqrt(d.x * d.x + d.z * d.z))
                .text("\n");
            builder.sText(TB::GRAY, " - ")
                .text("Manhattan: ")
                .num(abs(d.x) + abs(d.y) + abs(d.z))
                .text(" / ")
                .num(abs(d.x) + abs(d.z));
            return builder.get();
        }
    }  // namespace

    ActionResult setFirstPos(Player *player, const BlockPos &p1) {
        if (!player) return ErrorPlayerNeed();
        auto pos = p1 == BlockPos::MAX ? getLookAtPos(player) : p1;
        if (pos == BlockPos::MAX) {
            return {"no position specified", false};
        }
        positionCache()[player->getRealName()] = pos;
        return {fmt::format("First position was set to [{}]", pos.toString()), true};
    }
    ActionResult getDistance(Player *player, const BlockPos &p1, const BlockPos &p2) {
        // dis set 选点
        // dis 选点坐标和指针指向坐标距离
        // dis p1 选点坐标和p1的距离
        // dis p1 p2 p1和p2的距离
        if (p1 != BlockPos::MAX && p2 != BlockPos::MAX) {
            return {getDistanceInfo(p1, p2), true};
        }

        if (!player) return ErrorPlayerNeed();

        auto firstPosIter = positionCache().find(player->getRealName());
        if (firstPosIter == positionCache().end()) {
            return {"Use /dis set to specific the first position", false};
        }
        auto second = p1 == BlockPos::MAX ? getLookAtPos(player) : p1;
        if (second == BlockPos::MAX) {
            return {"You must specific the second position", false};
        }
        return {getDistanceInfo(firstPosIter->second, second), true};
    }
}  // namespace trapdoor
