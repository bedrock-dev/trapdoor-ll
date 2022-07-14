//
// Created by xhy on 2022/5/18.
//
#include "CommandHelper.h"

#include <MC/BlockInstance.hpp>
#include <MC/Player.hpp>

#include "DynamicCommandAPI.h"
#include "TBlockPos.h"

namespace trapdoor {
    ActionResult::ActionResult(std::string m, bool su) : msg(std::move(m)), success(su) {}
    void ActionResult::sendTo(CommandOutput & output) const {
        success ? output.success(msg) : output.error(msg);
    }

    ActionResult ErrorMsg(const std::string &msg) { return {msg, false}; }

    Vec3 getLookAtVec3(Player * player) {
        if (!player) {
            return Vec3::MAX;
        }
        auto *a = reinterpret_cast<Actor *>(player);
        auto *target = a->getActorFromViewVector(5.25);
        if (target) return target->getPos();
        auto ins = a->getBlockFromViewVector();
        return ins.isNull() ? Vec3::MAX : ins.getPosition().toVec3();
    }

    BlockPos getLookAtPos(Player * player) {
        if (!player) {
            return Vec3::MAX;
        }

        auto b = reinterpret_cast<Actor *>(player)->getBlockFromViewVector();
        return b.isNull() ? Vec3::MAX : b.getPosition();
    }
}  // namespace trapdoor