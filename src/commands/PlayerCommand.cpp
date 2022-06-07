#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "SimPlayerHelper.h"
#include "TrapdoorMod.h"

namespace tr {

    void setup_playerCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand(
            "player", "sapwn sim player",
            static_cast<CommandPermissionLevel>(level));

        auto spawnOpt = command->setEnum("spawnOpt", {"spawn", "despawn"});
        auto behOpt = command->setEnum("behOpt", {"lookat", "moveto"});
        auto intOpt = command->setEnum("intOpt", {"interact"});
        auto destroyopt = command->setEnum("destroy", {"destroy"});
        auto attackopt = command->setEnum("attack", {"attack"});
        auto jumpopt = command->setEnum("jump", {"jump"});
        auto repeatOpt = command->setEnum("repeatOpt", {"repeat"});
        auto useOpt = command->setEnum("useOpt", {"useslot"});
        auto useOnOpt = command->setEnum("useOnOpt", {"usesloton"});
        auto bagpackOpt = command->setEnum("backpackOpt", {"bagpack"});
        auto stopOpt = command->setEnum("stopOpt", {"stop", "cancel"});

        command->mandatory("player", ParamType::Enum, spawnOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, behOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, intOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, destroyopt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("player", ParamType::Enum, attackopt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, jumpopt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("player", ParamType::Enum, useOnOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, useOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("player", ParamType::Enum, bagpackOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, stopOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("playerName", ParamType::String);
        command->mandatory("slot", ParamType::Int);
        command->optional("vec3", ParamType::Vec3);
        command->optional("blockpos", ParamType::BlockPos);

        command->optional("repeatType", ParamType::Enum, repeatOpt,
                          CommandParameterOption::EnumAutocompleteExpansion);
        command->optional("interval", ParamType::Int);
        command->optional("times", ParamType::Int);

        command->optional("backpackslot", ParamType::Int);

        command->addOverload({"playerName", stopOpt});
        // check inv
        command->addOverload({"playerName", bagpackOpt, "backpackslot"});
        // spawn despawn
        command->addOverload({"playerName", spawnOpt});
        // move and loopat
        command->addOverload({"playerName", behOpt, "vec3"});
        //和方块/实体交互
        command->addOverload(
            {"playerName", intOpt, "repeatType", "interval", "times"});

        // destroy
        //    command->addOverload({"playerName", destroyopt, "blockpos"});
        command->addOverload({"playerName", destroyopt, "blockpos",
                              "repeatType", "interval", "times"});

        // use on

        command->addOverload({"playerName", useOnOpt, "slot", "blockpos",
                              "repeatType", "interval", "times"});

        command->addOverload(
            {"playerName", useOpt, "slot", "repeatType", "interval", "times"});

        command->addOverload(
            {"playerName", attackopt, "repeatType", "interval", "times"});

        command->addOverload(
            {"playerName", jumpopt, "repeatType", "interval", "times"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            auto name = results["playerName"].get<std::string>();
            //默认1gt一次
            int interval =
                results["interval"].isSet ? results["interval"].get<int>() : 1;
            //默认无限次
            int times =
                results["times"].isSet ? results["times"].get<int>() : -1;
            int rep = results["repeatType"].isSet ? 1 : 0;
            tr::logger().debug("repeat: {} , interval: {}  times:{}", rep,
                               interval, times);

            switch (do_hash(results["player"].getRaw<std::string>().c_str())) {
                case do_hash("spawn"):
                    tr::mod()
                        .sim_player_manager()
                        .addPlayer(name, origin.getBlockPosition(), 0)
                        .SendTo(output);
                    break;
                case do_hash("despawn"):
                    tr::mod().sim_player_manager().removePlayer(name).SendTo(
                        output);
                    break;
                case do_hash("lookat"):
                    tr::mod().sim_player_manager().behavior(
                        name, "lookat",
                        results["vec3"].isSet
                            ? results["vec3"].get<Vec3>()
                            : getLookAtPos(origin.getPlayer()));
                    break;
                case do_hash("moveto"):
                    tr::mod().sim_player_manager().behavior(
                        results["playerName"].getRaw<std::string>(), "moveto",
                        results["vec3"].isSet
                            ? results["vec3"].get<Vec3>()
                            : getLookAtPos(origin.getPlayer()));
                    break;
                case do_hash("interact"):
                    tr::mod()
                        .sim_player_manager()
                        .interactSchedule(name, origin.getPlayer(), rep,
                                          interval, times)
                        .SendTo(output);
                    break;

                case do_hash("destroy"):
                    tr::logger().debug("QAQ");
                    if (results["blockpos"].isSet) {
                        tr::mod().sim_player_manager().destroySchedule(
                            name, results["blockpos"].get<BlockPos>(), nullptr,
                            rep, interval, times);
                    } else {
                        tr::mod().sim_player_manager().destroySchedule(
                            name, BlockPos(0, 0, 0), origin.getPlayer(), rep,
                            interval, times);
                    }
                    break;

                case do_hash("bagpack"):
                    tr::mod().sim_player_manager().getBagpack(name, 0).SendTo(
                        output);
                    break;
                case do_hash("useslot"):
                    tr::mod().sim_player_manager().useItemOnSlot(
                        name, results["slot"].get<int>());
                    break;
                case do_hash("usesloton"):
                    if (results["blockpos"].isSet) {
                        tr::mod().sim_player_manager().useItemOnBlock(
                            name, results["slot"].get<int>(),
                            results["blockpos"].get<BlockPos>(), nullptr);
                    } else {
                        tr::mod().sim_player_manager().useItemOnBlock(
                            name, results["slot"].get<int>(), BlockPos(0, 0, 0),
                            origin.getPlayer());
                    }
                    break;
                case do_hash("cancel"):
                    tr::mod().sim_player_manager().cancel(name);
                    break;
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr
