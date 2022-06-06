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

        auto repeatOpt = command->setEnum("repeatOpt", {"repeat"});

        command->mandatory("player", ParamType::Enum, spawnOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, behOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, intOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, destroyopt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("playerName", ParamType::String);
        command->optional("vec3", ParamType::Vec3);
        command->optional("blockpos", ParamType::Vec3);

        command->optional("repeatType", ParamType::Enum, repeatOpt,
                          CommandParameterOption::EnumAutocompleteExpansion);
        command->optional("interval", ParamType::Int);
        command->optional("times", ParamType::Int);

        command->addOverload({"playerName", spawnOpt});
        command->addOverload({"playerName", behOpt, "vec3"});
        command->addOverload({"playerName", intOpt});

        command->addOverload(
            {"playerName", destroyopt, "blockpos", "repeatType"});
        command->addOverload({"playerName", destroyopt, "blockpos",
                              "repeatType", "interval", "times"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            auto name = results["playerName"].getRaw<std::string>();

            switch (do_hash(results["player"].getRaw<std::string>().c_str())) {
                case do_hash("spawn"):
                    tr::mod()
                        .sim_player_manager()
                        .addPlayer(results["playerName"].getRaw<std::string>(),
                                   origin.getBlockPosition(), 0)
                        .SendTo(output);
                    break;
                case do_hash("despawn"):
                    tr::mod()
                        .sim_player_manager()
                        .removePlayer(
                            results["playerName"].getRaw<std::string>())
                        .SendTo(output);
                    break;
                case do_hash("lookat"):
                    tr::mod().sim_player_manager().behavior(
                        results["playerName"].getRaw<std::string>(), "lookat",
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
                    tr::mod().sim_player_manager().interact(name,
                                                            origin.getPlayer());
                    break;
                case do_hash("destroy"):
                    tr::logger().debug("QAQ");
                    if (results["blockpos"].isSet) {
                        tr::mod().sim_player_manager().destroy(
                            name, results["blockpos"].get<BlockPos>(), nullptr);
                    } else {
                        tr::mod().sim_player_manager().destroy(
                            name, BlockPos(0, 0, 0), origin.getPlayer());
                    }
                    break;
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr
