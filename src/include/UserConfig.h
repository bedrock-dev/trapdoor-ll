
//
// Created by xhy on 2023/2/5.
//
#include <unordered_map>
#ifndef TRAPDOOR_USERCONFIG_H
#define TRAPDOOR_USERCONFIG_H
#include <filesystem>
class UserConfig {
   public:
    struct PlayerData {
        bool creativeNoClip;
        bool enableAutoTool;
        std::vector<int> hud_config;
    };

    // 暴露给外部的接口
    void setHUD(const std::string &name, int item, bool value) {}

    // 创造无碰撞
    void setCreativeClip(const std::string &name, bool value) {}
    // 自动切换工具
    void setAutoTool(const std::string &name, bool value) {}

   private:
    void syncConfigToDisk();
    std::unordered_map<std::string, PlayerData> playerData;

   public:
    void init() {}
};

#endif  // TRAPDOOR_USERCONFIG_H
