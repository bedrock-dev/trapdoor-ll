//
// Created by xhy on 2022/5/18.
//
#include "CommandHelper.h"

#include <MC/BlockInstance.hpp>
#include <MC/Player.hpp>

#include "DynamicCommandAPI.h"

namespace tr {

    ActionResult::ActionResult(std::string m, bool su)
        : msg(std::move(m)), success(su) {}
    void ActionResult::sendTo(CommandOutput &output) const {
        success ? output.success(msg) : output.error(msg);
    }

    ActionResult ErrorMsg(const std::string &msg) { return {msg, false}; }

    Vec3 getLookAtPos(Player *player) {
        if (!player) {
            return {0, 0, 0};
        }
        auto *a = reinterpret_cast<Actor *>(player);
        auto *target = a->getActorFromViewVector(5.25);
        if (target) return target->getPos();
        auto ins = a->getBlockFromViewVector();
        if (ins.isNull()) {
            return player->getPos();
        } else {
            return ins.getPosition().toVec3();
        }
    }

}  // namespace tr