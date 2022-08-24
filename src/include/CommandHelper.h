//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_COMMAND_HELPER_H
#define TRAPDOOR_COMMAND_HELPER_H

//clang-format off

#include <MC/Player.hpp>
#include <MC/Vec3.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "Global.h"

//clang-format on

class CommandOutput;
namespace trapdoor {
    struct ActionResult {
        std::string msg;
        bool success;

        ActionResult(std::string m, bool su);

        void sendTo(CommandOutput &output) const;
    };

    Vec3 getLookAtVec3(Player *player);

    BlockPos getLookAtPos(Player *player);

    ActionResult ErrorMsg(const std::string &msg);

    inline ActionResult ErrorPlayerNeed() {
        return {"This command require a player as executor", false};
    }

    inline ActionResult ErrorDimension() {
        return {"Can not get a dimension instance,try run this command in game", false};
    }

    inline ActionResult ErrorRange(const std::string &name, int begin, int end) {
        return {fmt::format("Error value {}, it should within [{},{}] ", name, begin, end), false};
    }

    inline ActionResult ErrorPosition() { return {"A position needed", false}; }

    void registerShortcutCommand(const std::string &shortcut,
                                 const std::vector<std::string> &actions);
    void setup_tickCommand(int);
    void setup_profCommand(int);
    void setup_villageCommand(int);
    void setup_logCommand(int);
    void setup_testCommand(int);
    void setup_spawnCommand(int);
    void setup_dataCommand(int);
    void setup_hsaCommand(int);
    void setup_trapdoorCommand(int);
    void setup_counterCommand(int);
    void setup_funcCommand(int);
    void setup_playerCommand(int);
    void setup_hudCommand(int);
    void setup_tweakCommand(int);
    void setup_CalCommand(int);
    void setup_disCommand(int);
    void setup_seedCommand(int);
    void setup_slimeCommand(int);
    void setup_triggerCommand(int);

}  // namespace trapdoor

#endif  // TRAPDOOR_COMMAND_HELPER_H
