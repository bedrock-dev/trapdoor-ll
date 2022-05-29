
#include <MC/BlockSource.hpp>
#include <MC/Dimension.hpp>
#include <MC/Player.hpp>
#include <MC/ServerPlayer.hpp>
#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "InfoDisplayer.h"

namespace tr {
    void SetupDataCommand() {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand(
            "data", "get game data", CommandPermissionLevel::GameMasters);

        auto &blockSubCommandEnum =
            command->setEnum("blockSubCommand", {"block"});
        auto &entitySubCommand =
            command->setEnum("entitySubCommand", {"entity"});
        auto &redstoneSubCommand =
            command->setEnum("redstoneSubCommand", {"redstone"});

        //给根命令+enum提示信息
        command->mandatory("data", ParamType::Enum, blockSubCommandEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("data", ParamType::Enum, entitySubCommand,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("data", ParamType::Enum, redstoneSubCommand,
                           CommandParameterOption::EnumAutocompleteExpansion);

        //设置每个enum后面需要的参数类型
        command->optional("blockPos", ParamType::BlockPos);

        //添加子命令并进行类型绑定
        command->addOverload({blockSubCommandEnum, "blockPos"});
        command->addOverload({entitySubCommand});
        command->addOverload({redstoneSubCommand, "blockPos"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            switch (do_hash(results["data"].getRaw<std::string>().c_str())) {
                case do_hash("block"):

                    if (results["blockPos"].isSet) {
                        tr::displayerBlockInfo(
                            origin.getPlayer(),
                            results["blockPos"].get<BlockPos>());
                    } else {
                        tr::displayerBlockInfo(
                            origin.getPlayer(),
                            reinterpret_cast<Actor *>(origin.getPlayer())
                                ->getBlockFromViewVector()
                                .getPosition());
                    }

                    break;
                case do_hash("entity"):
                    tr::displayEntityInfo(
                        origin.getPlayer(),
                        reinterpret_cast<Actor *>(origin.getPlayer())
                            ->getActorFromViewVector(10));
                    break;
                case do_hash("redstone"):
                    if (results["blockPos"].isSet) {
                        tr::displayerRedstoneCompInfo(
                            origin.getPlayer(),
                            results["blockPos"].get<BlockPos>());
                    } else {
                        tr::displayerRedstoneCompInfo(
                            origin.getPlayer(),
                            reinterpret_cast<Actor *>(origin.getPlayer())
                                ->getBlockFromViewVector()
                                .getPosition());
                    }
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr
