
//
// Created by xhy on 2023/2/5.
//
#include <unordered_map>

#ifndef TRAPDOOR_USERCONFIG_H
#define TRAPDOOR_USERCONFIG_H

#include <filesystem>
#include "HUDHelper.h"
#include <mc/Level.hpp>

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

            //内存数据,不会持久化
            int lastActiveChannel = -1;
        };

    public:

        //set user data


#define  SET_FUNC_NAME(item) set_##item
#define  GET_FUNC_NAME(item) get_##item

#define  VALUE_FUNC_NAME(item) item

#define  ADD_SET_FUNC(item, type) ActionResult SET_FUNC_NAME(item) (const std::string &player, type value) \
        {this->playerData[player].item = value; this->syncConfigToDisk(player);  return trapdoor::SetValueMsg(#item,value); }

#define  ADD_GET_FUNC(item) ActionResult GET_FUNC_NAME(item) (const std::string &player) \
        { return trapdoor::GetValueMsg(#item,this->playerData[player].item); }

#define  ADD_VALUE_FUNC(item) auto VALUE_FUNC_NAME(item) (const std::string &player) \
        {return this->playerData[player].item;}


#define  ADD_FUNC(item, type)  ADD_SET_FUNC(item,type)   ADD_GET_FUNC(item) ADD_VALUE_FUNC(item)

        ADD_FUNC(hud, bool)

//        ADD_FUNC(noclip, bool)

        ADD_FUNC(blockrotate, bool)

        ADD_FUNC(autotool, bool)

        // ADD_FUNC(fcopen, bool)

        ActionResult set_noclip(const std::string &name, bool value);

        ActionResult get_noclip(const std::string &player) {
            return trapdoor::GetValueMsg("noclip", this->playerData[player].noclip);
        }

        auto noclip(const std::string &player) { return this->playerData[player].noclip; }


        void setHUD(const std::string &name, int item, bool value);

    public:

        [[nodiscard]] auto &getPlayerData() const { return this->playerData; }

        // 暴露给外部的接口

        ActionResult dumpAllSelfConfig(const std::string &name);


        int getActiveHopperChannel(const std::string &name) { return this->playerData[name].lastActiveChannel; }

        void setActiveHopperChannel(const std::string &name,
                                    int channel) { this->playerData[name].lastActiveChannel = channel; }

    private:
        void syncConfigToDisk(const std::string &name);

        std::unordered_map<std::string, PlayerData> playerData;

    public:
        void init();


    };

}
#endif  // TRAPDOOR_USERCONFIG_H
