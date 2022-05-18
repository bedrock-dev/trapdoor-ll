//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_SETTINGS_H
#define TRAPDOOR_SETTINGS_H

#include "MC/Command.hpp"

#include "MC/Level.hpp"
#include "MC/ServerPlayer.hpp"
#include "GlobalServiceAPI.h"

namespace tr {

    //TODO 设计配置文件的结构
    struct CommandConfig {
        bool enable = true;
        CommandPermissionLevel permissionLevel = GameMasters;
        GameType type = static_cast<GameType>(0);
    };

    struct FunctionConfig {


    };


    struct VillageConfig {

    };


    class Settings {
    };
}


#endif //TRAPDOOR_SETTINGS_H
