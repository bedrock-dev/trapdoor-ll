
#include <MC/BlockInstance.hpp>
#include <MC/Dimension.hpp>
#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "SpawnHelper.h"

namespace tr {
    void setup_spawnCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand(
            "spawn", "print spawn info",
            static_cast<CommandPermissionLevel>(level));

        // set enum就是给这个命令加一些enum值，不会产生任何意义
        auto &optCount = command->setEnum("Countcmd", {"count"});
        auto &prob = command->setEnum("probilitycmd", {"prob"});
        auto &forcesp = command->setEnum("forcecmd", {"forcesp"});

        auto &optCountType =
            command->setEnum("counter options", {"chunk", "all", "density"});

        // mandatory/options就是给enum增加后置参数类型的,mandatory就是必填,optional是选填
        command->mandatory("spawn", ParamType::Enum, optCount,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("spawn", ParamType::Enum, prob,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("spawn", ParamType::Enum, forcesp,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("countType", ParamType::Enum, optCountType,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("actorType", ParamType::ActorType);

        command->optional("blockPos", ParamType::BlockPos);

        //添加子命令并进行类型绑定
        command->addOverload({prob, "blockPos"});
        command->addOverload({forcesp, "actorType"});

        // add
        // overload就是增加一些子命令，子命令需要Enum；并设定后面需要接什么类型的参数
        command->addOverload({optCount, "countType"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            auto countParam = std::string();
            switch (do_hash(results["spawn"].getRaw<std::string>().c_str())) {
                case do_hash("count"):
                    tr::countActors(reinterpret_cast<Player *>(origin.getPlayer()),
                                    results["countType"].getRaw<std::string>())
                        .sendTo(output);
                    break;

                case do_hash("forcesp"):
                    tr::forceSpawn(

                        reinterpret_cast<Player *>(origin.getPlayer()),
                        results["actorType"].get<const ActorDefinitionIdentifier *>(),
                        reinterpret_cast<Actor *>(origin.getPlayer())
                            ->getBlockFromViewVector()
                            .getPosition()

                    );
                    break;
                case do_hash("prob"):
                    if (results["blockPos"].isSet) {
                        tr::printSpawnProbability(reinterpret_cast<Player *>(origin.getPlayer()),
                                                  results["blockPos"].get<BlockPos>())
                            .sendTo(output);
                    } else {
                        tr::printSpawnProbability(reinterpret_cast<Player *>(origin.getPlayer()),
                                                  reinterpret_cast<Actor *>(origin.getPlayer())
                                                      ->getBlockFromViewVector()
                                                      .getPosition())
                            .sendTo(output);
                    }
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr