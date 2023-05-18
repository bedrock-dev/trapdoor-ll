//
// Created by xhy on 2022/7/15.
//
#include "DistanceHelper.h"
#include "DynamicCommandAPI.h"
#include "SpawnHelper.h"
#include "TrapdoorMod.h"

namespace trapdoor {
    // 距离计算
    const DynamicCommandInstance * setup_disCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command

        auto command = CREATE_CMD(dis, level);

        auto &selectPos = command->setEnum("setPos", {"set"});

        command->mandatory("dis", ParamType::Enum, selectPos,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->optional("p1", ParamType::BlockPos);
        command->optional("p2", ParamType::BlockPos);
        // dis p1/p2 选点
        // dis 计算选择的两点p1,p2的距离
        // dis 计算p1和p2的距离

        /**
         * dis <set> 选一个点
         * dis [pos] 选择的点和输入点的距离，不输入表示输入指针指向的方块
         * dis pos1 pos2 p1和p2的距离
         */

        command->addOverload({selectPos, "p1"});
        command->addOverload({"p1", "p2"});
        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            auto p1 = results["p1"].isSet ? results["p1"].get<BlockPos>() : BlockPos::MAX;
            auto p2 = results["p2"].isSet ? results["p2"].get<BlockPos>() : BlockPos::MAX;
            if (!results["dis"].isSet) {
                trapdoor::getDistance(origin.getPlayer(), p1, p2).sendTo(output);
                return;
            }
            switch (do_hash(results["dis"].getRaw<std::string>().c_str())) {
                case do_hash("set"):
                    trapdoor::setFirstPos(origin.getPlayer(), p1).sendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
      return  DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor