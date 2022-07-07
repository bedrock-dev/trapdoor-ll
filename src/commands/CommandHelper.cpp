//
// Created by xhy on 2022/5/18.
//
#include "CommandHelper.h"

#include <MC/BlockInstance.hpp>
#include <MC/Player.hpp>

#include "DynamicCommandAPI.h"
#include "TBlockPos.h"

namespace tr {

    ActionResult::ActionResult(std::string m, bool su) : msg(std::move(m)), success(su) {}
    void ActionResult::sendTo(CommandOutput &output) const {
        success ? output.success(msg) : output.error(msg);
    }

    ActionResult ErrorMsg(const std::string &msg) { return {msg, false}; }

    Vec3 getLookAtVec3(Player *player) {
        if (!player) {
            return tr::INVALID_VEC3;
        }
        auto *a = reinterpret_cast<Actor *>(player);
        auto *target = a->getActorFromViewVector(5.25);
        if (target) return target->getPos();
        auto ins = a->getBlockFromViewVector();
        return ins.isNull() ? INVALID_VEC3 : ins.getPosition().toVec3();
    }

    BlockPos getLookAtPos(Player *player) {
        if (!player) {
            return tr::INVALID_POS;
        }

        auto b = reinterpret_cast<Actor *>(player)->getBlockFromViewVector();
        return b.isNull() ? tr::INVALID_POS : b.getPosition();
    }
}  // namespace tr