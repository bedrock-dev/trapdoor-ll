//
// Created by xhy on 2022/5/17.
//
#include <MC/Player.hpp>
#include <MC/ServerPlayer.hpp>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "Msg.h"
namespace tr {

    namespace {
        void setPlayerGameType(ServerPlayer *sp, int type) {
            if (!sp) return;
            sp->runcmd("gamemode " + std::to_string(type));
            auto gameType = "unknown";
            if (type == 0) gameType = "survival";
            if (type == 1) gameType = "creative";

            tr::BroadcastMessage(
                "Change player " + sp->getRealName() + " 's game type to " + gameType + " mode",
                -1);
        }
    }  // namespace

    void setup_oCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("o", "Quick switch to observation mode",
                                                     static_cast<CommandPermissionLevel>(level));
        command->addOverload(std::vector<std::string>{});
        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            setPlayerGameType(origin.getPlayer(), 3);
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

    void setup_sCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("s", "Quick switch to survival mode",
                                                     static_cast<CommandPermissionLevel>(level));
        command->addOverload(std::vector<std::string>{});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            setPlayerGameType(origin.getPlayer(), 0);
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

    void setup_cCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("c", "Quick switch to create mode",
                                                     static_cast<CommandPermissionLevel>(level));

        command->addOverload(std::vector<std::string>{});
        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            setPlayerGameType(origin.getPlayer(), 1);
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace tr