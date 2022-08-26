
#include <MC/BlockSource.hpp>
#include <MC/Dimension.hpp>
#include <MC/Player.hpp>
#include <MC/ServerPlayer.hpp>
#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "InfoDisplay.h"
#include "RedstoneTool.h"

namespace trapdoor {
    void setup_dataCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("data", "print some game data",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &blockSubCommandEnum = command->setEnum("blockSubCommand", {"block"});
        auto &entitySubCommand = command->setEnum("entitySubCommand", {"entity"});
        auto &redstoneSubCommand = command->setEnum("redstoneSubCommand", {"redstone"});
        auto nbtEnum = command->setEnum("nbt", {"nbt"});

        auto redstoneEnum = command->setEnum("redstone", {"chunk", "conn", "signal"});

        // 给根命令+enum提示信息
        command->mandatory("data", ParamType::Enum, blockSubCommandEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("data", ParamType::Enum, entitySubCommand,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("data", ParamType::Enum, redstoneSubCommand,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("redstone", ParamType::Enum, redstoneEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);

        // 设置每个enum后面需要的参数类型
        command->optional("blockPos", ParamType::BlockPos);
        command->optional("nbt", ParamType::Enum, nbtEnum,
                          CommandParameterOption::EnumAutocompleteExpansion);
        command->optional("path", ParamType::String);

        // 添加子命令并进行类型绑定
        command->addOverload({blockSubCommandEnum, "blockPos", "nbt", "path"});
        command->addOverload({entitySubCommand, "nbt", "path"});
        command->addOverload({redstoneSubCommand, "redstone", "blockPos"});

        //   command->addOverload({redstoneSubCommand, "blockPos"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            bool displayNBT = results["nbt"].isSet;
            auto nbtPath = results["path"].getRaw<std::string>();
            auto blockPos =
                results["blockPos"].isSet ? results["blockPos"].get<BlockPos>() : BlockPos::MAX;

            auto redstoneOpt = results["redstone"].get<std::string>();

            switch (do_hash(results["data"].getRaw<std::string>().c_str())) {
                case do_hash("block"):
                    trapdoor::displayBlockInfo(origin.getPlayer(), blockPos, displayNBT, nbtPath)
                        .sendTo(output);
                    break;
                case do_hash("entity"):
                    trapdoor::displayEntityInfo(
                        origin.getPlayer(),
                        reinterpret_cast<Actor *>(origin.getPlayer())->getActorFromViewVector(10),
                        displayNBT, nbtPath)
                        .sendTo(output);
                    break;
                case do_hash("redstone"):
                    if (results["blockPos"].isSet) {
                        trapdoor::displayRedstoneCompInfo(
                            origin.getDimension(), results["blockPos"].get<BlockPos>(), redstoneOpt)
                            .sendTo(output);

                    } else {
                        if (!origin.getPlayer()) {
                            ErrorPlayerNeed().sendTo(output);
                        } else {
                            trapdoor::displayRedstoneCompInfo(origin.getDimension(),
                                                              getLookAtPos(origin.getPlayer()),
                                                              redstoneOpt)
                                .sendTo(output);
                        }
                    }
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor
