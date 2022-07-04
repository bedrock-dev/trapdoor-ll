#include <MC/Dimension.hpp>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "SimPlayerHelper.h"
#include "TrapdoorMod.h"

namespace tr {

    void setup_playerCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("player", "sapwn simPlayer player",
                                                     static_cast<CommandPermissionLevel>(level));

        auto spawnOpt = command->setEnum("spawnOpt", {"spawn", "despawn"});
        auto behOpt = command->setEnum("behOpt", {"lookat", "moveto"});
        auto intOpt = command->setEnum("intOpt", {"interact"});
        auto destroyOpt = command->setEnum("destroy", {"destroy"});
        auto attackOpt = command->setEnum("attack", {"attack"});
        auto jumpOpt = command->setEnum("jump", {"jump"});
        auto repeatOpt = command->setEnum("repeatOpt", {"repeat"});
        auto useOpt = command->setEnum("useOpt", {"use"});
        auto useOnOpt = command->setEnum("useOnOpt", {"useon"});
        auto backpackOpt = command->setEnum("backpackOpt", {"backpack"});
        auto stopOpt = command->setEnum("stopOpt", {"stop", "cancel"});
        auto setOpt = command->setEnum("setOpt", {"set", "drop"});

        command->mandatory("player", ParamType::Enum, spawnOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, behOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, intOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, destroyOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("player", ParamType::Enum, attackOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, jumpOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("player", ParamType::Enum, useOnOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, useOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("player", ParamType::Enum, backpackOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, stopOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("player", ParamType::Enum, setOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        //      command->mandatory("name", ParamType::String);
        //        command->addSoftEnumValues("name", {});
        command->mandatory("name", ParamType::SoftEnum, command->setSoftEnum("name", {}));

        command->mandatory("itemId", ParamType::Item);
        command->optional("vec3", ParamType::Vec3);
        command->optional("blockPos", ParamType::BlockPos);

        command->optional("repeatType", ParamType::Enum, repeatOpt,
                          CommandParameterOption::EnumAutocompleteExpansion);
        command->optional("interval", ParamType::Int);
        command->optional("times", ParamType::Int);

        command->optional("slot", ParamType::Int);

        // clang-format off
        //  cancel task and stop action
        command->addOverload({"name", stopOpt});
        // check inv
        command->addOverload({"name", backpackOpt, "slot"});
        // spawn despawn
        command->addOverload({"name", setOpt, "itemId"});

        command->addOverload({"name", spawnOpt});
        // move and loopat
        command->addOverload({"name", behOpt, "vec3"});
        //和方块/实体交互
        command->addOverload({"name", intOpt, "repeatType", "interval","times"});
        // destroy
        command->addOverload({"name", destroyOpt, "blockPos", "repeatType", "interval", "times"});
        // useitem
        command->addOverload({"name", useOnOpt, "itemId", "blockPos", "repeatType", "interval", "times"});
        //use item on
        command->addOverload({"name", useOpt, "itemId", "repeatType", "interval", "times"});
        //attack
        command->addOverload({"name", attackOpt, "repeatType", "interval", "times"});
        //jump
        command->addOverload({"name", jumpOpt, "repeatType", "interval", "times"});


        command->addOverload(std::vector<std::string>());

        // clang-format on

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            auto name = results["name"].isSet ? results["name"].get<std::string>() : std::string();
            if (name.empty()) {
                tr::mod().getSimPlayerManager().listAll().sendTo(output);
                return;
            }

            // 默认1gt一次
            int interval = results["interval"].isSet ? results["interval"].get<int>() : 1;
            // 默认无限次
            int times = results["times"].isSet ? results["times"].get<int>() : -1;
            int rep = results["repeatType"].isSet ? 1 : 0;
            int itemId =
                results["itemId"].isSet ? results["itemId"].getRaw<CommandItem>().getId() : 0;
            int slot = results["slot"].isSet ? results["slot"].get<int>() : -1;
            auto blockPos =
                results["blockPos"].isSet ? results["blockPos"].get<BlockPos>() : tr::INVALID_POS;

            switch (do_hash(results["player"].getRaw<std::string>().c_str())) {
                case do_hash("spawn"):
                    tr::mod()
                        .getSimPlayerManager()
                        .addPlayer(name, origin.getBlockPosition(),
                                   origin.getDimension()->getDimensionId(), origin.getPlayer())
                        .sendTo(output);
                    break;
                case do_hash("despawn"):
                    tr::mod().getSimPlayerManager().removePlayer(name).sendTo(output);
                    break;

                case do_hash("lookat"):
                    tr::mod()
                        .getSimPlayerManager()
                        .behavior(name, "lookat",
                                  results["vec3"].isSet ? results["vec3"].get<Vec3>()
                                                        : getLookAtVec3(origin.getPlayer()))
                        .sendTo(output);
                    break;
                case do_hash("set"):
                    tr::mod().getSimPlayerManager().setItem(name, itemId).sendTo(output);
                    break;
                case do_hash("drop"):
                    tr::mod().getSimPlayerManager().dropItem(name, itemId).sendTo(output);
                    break;
                case do_hash("moveto"):
                    tr::mod()
                        .getSimPlayerManager()
                        .behavior(name, "moveto",
                                  results["vec3"].isSet ? results["vec3"].get<Vec3>()
                                                        : getLookAtVec3(origin.getPlayer()))
                        .sendTo(output);
                    break;

                case do_hash("interact"):
                    tr::mod()
                        .getSimPlayerManager()
                        .interactSchedule(name, origin.getPlayer(), rep, interval, times)
                        .sendTo(output);
                    break;

                case do_hash("attack"):
                    tr::mod()
                        .getSimPlayerManager()
                        .attackSchedule(name, origin.getPlayer(), rep, interval, times)
                        .sendTo(output);
                    break;

                case do_hash("jump"):
                    tr::mod()
                        .getSimPlayerManager()
                        .jumpSchedule(name, rep, interval, times)
                        .sendTo(output);
                    break;

                case do_hash("destroy"):
                    tr::mod()
                        .getSimPlayerManager()
                        .destroySchedule(name, blockPos, origin.getPlayer(), rep, interval, times)
                        .sendTo(output);
                    break;

                case do_hash("backpack"):
                    tr::mod().getSimPlayerManager().getBackpack(name, 0).sendTo(output);
                    break;

                // use
                case do_hash("use"):
                    tr::mod()
                        .getSimPlayerManager()
                        .useSchedule(name, itemId, rep, interval, times)
                        .sendTo(output);
                    break;
                case do_hash("useon"):
                    if (results["blockPos"].isSet) {
                        tr::mod()
                            .getSimPlayerManager()
                            .useOnBlockSchedule(name, itemId, results["blockPos"].get<BlockPos>(),
                                                nullptr, rep, interval, times)
                            .sendTo(output);
                    } else {
                        tr::mod()
                            .getSimPlayerManager()
                            .useOnBlockSchedule(name, itemId, BlockPos(0, 0, 0), origin.getPlayer(),
                                                rep, interval, times)
                            .sendTo(output);
                    }

                    break;
                case do_hash("cancel"):
                    tr::mod().getSimPlayerManager().cancel(name);
                    break;
                case do_hash("stop"):
                    tr::mod().getSimPlayerManager().stopAction(name);
                    break;
            }
        };

        command->setCallback(cb);
        auto cmd = DynamicCommand::setup(std::move(command));
        tr::mod().getSimPlayerManager().setupCommandInstance(cmd);
    }
}  // namespace tr
