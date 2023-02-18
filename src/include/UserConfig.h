
//
// Created by xhy on 2023/2/5.
//
#include <unordered_map>

#ifndef TRAPDOOR_USERCONFIG_H
#define TRAPDOOR_USERCONFIG_H

#include <filesystem>
#include "HUDHelper.h"

namespace trapdoor {

    /**
    - 创造模式无碰撞 noclip
    - 仙人掌转方块 blockrotate
    - 自动换工具 autotool
    - 强制开容 fcopen
     */

    class UserConfig {
    public:
        struct PlayerData {
            bool hud{true};
            bool noclip{true};
            bool blockrotate{true};
            bool autotool{true};
            bool fcopen{true};
            std::array<bool, HUDItemType::LEN> hud_config{};
        };


    public:

        //set user data


#define  FUNC_NAME(item) set_##item

#define  ADD_SET_FUNC(item, type) ActionResult FUNC_NAME(item) (const std::string &player, type value) \
        {this->playerData[player].item = value;  return trapdoor::OperationSuccess(); }

        ADD_SET_FUNC(hud, bool)

        ADD_SET_FUNC(noclip, bool)

        ADD_SET_FUNC(blockrotate, bool)

        ADD_SET_FUNC(autotool, bool)

        ADD_SET_FUNC(fcopen, bool)


        void setHUD(const std::string &name, int item, bool value);

    public:

        [[nodiscard]] auto &getPlayerData() const { return this->playerData; }

        // 暴露给外部的接口



    private:
        void syncConfigToDisk(const std::string &name);

        std::unordered_map<std::string, PlayerData> playerData;

    public:
        void init();


    };

}
#endif  // TRAPDOOR_USERCONFIG_H
