//
// Created by xhy on 2022/5/17.
//
#include "DynamicCommandAPI.h"
#include "TrapdoorMod.h"
#include "VillageHelper.h"

namespace tr {
    void setup_villageCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand(
            "village", "village helper",
            static_cast<CommandPermissionLevel>(level));

        auto &optSwitch = command->setEnum(
            "optSwitch", {"bound", "spawn", "center", "poi", "head"});
        command->mandatory("village", ParamType::Enum, optSwitch,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("onOroff", ParamType::Bool);
        command->addOverload({optSwitch, "onOroff"});

        auto &optList = command->setEnum("optList", {"list"});
        command->mandatory("village", ParamType::Enum, optList,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optList});

        auto &optOther = command->setEnum("optOther", {"info"});
        command->optional("village", ParamType::Enum, optOther,
                          CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optOther});
        command->mandatory("villageID", ParamType::Int);
        command->addOverload({optOther, "villageID"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            switch (do_hash(results["village"].getRaw<std::string>().c_str())) {
                case do_hash("list"):
                    tr::mod().village_helper().ListTickingVillages(true).SendTo(
                        output);
                    break;

                case do_hash("bound"):
                    tr::mod()
                        .village_helper()
                        .ShowBounds(results["onOroff"].getRaw<bool>())
                        .SendTo(output);
                    break;

                case do_hash("spawn"):
                    tr::mod()
                        .village_helper()
                        .ShowIronSpawnArea(results["onOroff"].getRaw<bool>())
                        .SendTo(output);
                    break;

                case do_hash("center"):
                    tr::mod()
                        .village_helper()
                        .ShowCenter(results["onOroff"].getRaw<bool>())
                        .SendTo(output);
                    break;

                case do_hash("poi"):
                    tr::mod()
                        .village_helper()
                        .ShowPoiQury(results["onOroff"].getRaw<bool>())
                        .SendTo(output);
                    break;

                case do_hash("head"):
                    tr::mod()
                        .village_helper()
                        .ShowVillagerHeadInfo(results["onOroff"].getRaw<bool>())
                        .SendTo(output);

                    break;

                case do_hash("info"):
                    if (results["villageID"].isSet) {
                        tr::mod()
                            .village_helper()
                            .PrintDetails(results["villageID"].getRaw<int>(),
                                          Vec3::ZERO)
                            .SendTo(output);
                    } else {
                        tr::mod()
                            .village_helper()
                            .PrintDetails(-1, origin.getPlayer()->getPos())
                            .SendTo(output);
                    }
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace tr