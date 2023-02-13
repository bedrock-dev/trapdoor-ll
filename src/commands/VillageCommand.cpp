//
// Created by xhy on 2022/5/17.
//
#include "DynamicCommandAPI.h"
#include "TrapdoorMod.h"
#include "VillageHelper.h"

namespace trapdoor {
    void setup_villageCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        auto command = CREATE_CMD(village, level);
        auto &optSwitch =
            command->setEnum("optSwitch", {"bound", "spawn", "center", "poi", "head"});
        auto &optList = command->setEnum("optList", {"list"});
        auto &optOther = command->setEnum("optOther", {"info", "dweller"});
        command->mandatory("village", ParamType::Enum, optSwitch,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("village", ParamType::Enum, optList,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("village", ParamType::Enum, optOther,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("onoroff", ParamType::Bool);
        command->optional("vid", ParamType::SoftEnum, command->setSoftEnum("vid", {}));
        // command->mandatory("vid", ParamType::String);

        command->addOverload({optSwitch, "onoroff"});
        command->addOverload({optList});
        command->addOverload({optOther, "vid"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            auto show = results["onoroff"].getRaw<bool>();
            switch (do_hash(results["village"].getRaw<std::string>().c_str())) {
                case do_hash("list"):
                    trapdoor::mod().getVillageHelper().listTickingVillages(true).sendTo(output);
                    break;

                case do_hash("bound"):
                    trapdoor::mod().getVillageHelper().setShowBounds(show).sendTo(output);
                    break;

                case do_hash("spawn"):
                    trapdoor::mod().getVillageHelper().setShowIronSpawnArea(show).sendTo(output);
                    break;

                case do_hash("center"):
                    trapdoor::mod().getVillageHelper().setShowCenter(show).sendTo(output);
                    break;

                case do_hash("poi"):
                    trapdoor::mod().getVillageHelper().setShowPoiQuery(show).sendTo(output);
                    break;

                case do_hash("head"):
                    trapdoor::mod().getVillageHelper().setShowVillagerHeadInfo(show).sendTo(output);
                    break;
                case do_hash("info"):
                    if (results["vid"].isSet) {
                        trapdoor::mod()
                            .getVillageHelper()
                            .printDetails(results["vid"].getRaw<std::string>(), Vec3::ZERO)
                            .sendTo(output);
                    } else {
                        trapdoor::mod()
                            .getVillageHelper()
                            .printDetails("", origin.getPlayer()->getPos())
                            .sendTo(output);
                    }
                    break;

                case do_hash("dweller"):
                    trapdoor::mod()
                        .getVillageHelper()
                        .showVillageInfo(origin.getPlayer())
                        .sendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
        auto *cmd = DynamicCommand::setup(std::move(command));
        trapdoor::mod().getVillageHelper().setupCommandInstance(cmd);
    }

}  // namespace trapdoor