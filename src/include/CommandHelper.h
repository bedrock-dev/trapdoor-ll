//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_COMMAND_HELPER_H
#define TRAPDOOR_COMMAND_HELPER_H

//clang-format off

#include <mc/Player.hpp>
#include <mc/Vec3.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "Global.h"

//clang-format on

#define CREATE_CMD(cmd, level)                                \
    DynamicCommand::createCommand(#cmd, getCmdDescI18n(#cmd), \
                                  static_cast<CommandPermissionLevel>(level))

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

    ActionResult SuccessMsg(const std::string &msg);

    inline ActionResult ErrorPlayerNeed() { return ErrorMsg("command.error.player-required"); }

    inline ActionResult ErrorCreativeNeed() { return ErrorMsg("command.error.creative-required"); }

    inline ActionResult ErrorDimension() { return ErrorMsg("command.error.dimension-required"); }

    inline ActionResult ErrorUnexpected(const std::string &msg) { return ErrorMsg("Unexpected error" + msg); }

    inline ActionResult ErrorRange(const std::string &name, int begin, int end) {
        return {fmt::format(tr("command.error.range"), name, begin, end), false};
    }

    template<typename T>
    inline ActionResult SetValueMsg(const std::string &key, T value) {
        return {fmt::format(tr("command.success.set-value"), key, value), true};
    }

    template<typename T>
    inline ActionResult GetValueMsg(const std::string &key, T value) {
        return {fmt::format(tr("command.success.get-value"), key, value), true};
    }

    inline ActionResult OperationSuccess() { return SuccessMsg("command.operation.success"); }

    inline ActionResult ErrorPosition() { return ErrorMsg("command.error.position-required"); }

    inline ActionResult ErrorDeveloping() { return ErrorMsg("command.error.developing"); }

    void registerShortcutCommand(const std::string &shortcut,
                                 const std::vector<std::string> &actions);

    inline std::string getCmdDescI18n(const std::string &cmd) {
        return "§b" + tr("command." + cmd + ".desc");
    }


    //Command setup


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

    void setup_CalCommand(int);

    void setup_disCommand(int);

    // void setup_seedCommand(int);
    void setup_slimeCommand(int);

    void setup_triggerCommand(int);

    void setup_randCommand(int);

    void setup_selfCommand(int);

}  // namespace trapdoor

#endif  // TRAPDOOR_COMMAND_HELPER_H
