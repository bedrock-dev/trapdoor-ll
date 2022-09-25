
#include <mc/BlockInstance.hpp>
#include <mc/Dimension.hpp>
#include <string>

#include "CommandHelper.h"
#include "DataConverter.h"
#include "DynamicCommandAPI.h"
#include "SpawnHelper.h"
#include "TBlockPos.h"
#include "TrapdoorMod.h"
namespace trapdoor {
    void setup_spawnCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("spawn", "print spawn info",
                                                     static_cast<CommandPermissionLevel>(level));

        // set enum就是给这个命令加一些enum值，不会产生任何意义
        auto &optCount = command->setEnum("Countcmd", {"count"});
        auto &prob = command->setEnum("probilitycmd", {"prob"});
        auto &forcesp = command->setEnum("forcecmd", {"forcesp"});
        auto &optCountType = command->setEnum("counter options", {"chunk", "all", "density"});

        auto &analyzeOpt = command->setEnum("analyze", {"analyze"});
        auto &analyzeSubOpt =
            command->setEnum("analyze options", {"start", "stop", "print", "clear"});

        // mandatory/options就是给enum增加后置参数类型的,mandatory就是必填,optional是选填
        command->mandatory("spawn", ParamType::Enum, optCount,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("spawn", ParamType::Enum, prob,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("spawn", ParamType::Enum, forcesp,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("countType", ParamType::Enum, optCountType,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("spawn", ParamType::Enum, analyzeOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("analyzeSub", ParamType::Enum, analyzeSubOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("actorType", ParamType::ActorType);

        command->optional("blockPos", ParamType::BlockPos);

        // 添加子命令并进行类型绑定
        command->addOverload({prob, "blockPos"});
        command->addOverload({forcesp, "actorType", "blockPos"});
        // add
        // overload就是增加一些子命令，子命令需要Enum；并设定后面需要接什么类型的参数
        command->addOverload({optCount, "countType"});

        command->addOverload({analyzeOpt, "analyzeSub"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            auto countParam = std::string();
            auto subOpt =
                results["analyzeSub"].isSet ? results["analyzeSub"].getRaw<std::string>() : "";
            switch (do_hash(results["spawn"].getRaw<std::string>().c_str())) {
                case do_hash("count"):
                    trapdoor::countActors(reinterpret_cast<Player *>(origin.getPlayer()),
                                          results["countType"].getRaw<std::string>())
                        .sendTo(output);
                    break;
                case do_hash("forcesp"):
                    trapdoor::forceSpawn(
                        reinterpret_cast<Player *>(origin.getPlayer()),
                        results["actorType"].get<const ActorDefinitionIdentifier *>(),
                        results["blockPos"].isSet ? results["blockPos"].get<BlockPos>()
                                                  : BlockPos::MAX)
                        .sendTo(output);
                    break;
                case do_hash("analyze"):
                    if (subOpt == "start") {
                        trapdoor::mod()
                            .getSpawnAnalyzer()
                            .start(origin.getDimension()->getDimensionId(),
                                   fromBlockPos(origin.getBlockPosition()).toChunkPos())
                            .sendTo(output);
                    } else if (subOpt == "stop") {
                        trapdoor::mod().getSpawnAnalyzer().stop().sendTo(output);
                    } else if (subOpt == "print") {
                        trapdoor::mod().getSpawnAnalyzer().printResult().sendTo(output);
                    } else if (subOpt == "clear") {
                        trapdoor::mod().getSpawnAnalyzer().clear().sendTo(output);
                    }
                    break;
                case do_hash("prob"):
                    if (results["blockPos"].isSet) {
                        trapdoor::printSpawnProbability(
                            reinterpret_cast<Player *>(origin.getPlayer()),
                            results["blockPos"].get<BlockPos>())
                            .sendTo(output);
                    } else {
                        trapdoor::printSpawnProbability(
                            reinterpret_cast<Player *>(origin.getPlayer()),
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
}  // namespace trapdoor