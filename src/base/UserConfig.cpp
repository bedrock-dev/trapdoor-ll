//
// Created by xhy on 2023/2/5.
//

#include "UserConfig.h"
#include "TrapdoorMod.h"
#include "Nlohmann/json.hpp"
#include "HUDHelper.h"


namespace trapdoor {


    void UserConfig::syncConfigToDisk(const std::string &name) {

        auto it = this->playerData.find(name);
        if (it == this->playerData.end()) {
            //这里理论上不可能找不到
            trapdoor::logger().error("Can not find player config data in memory");
        }

        auto &data = it->second;

        using namespace nlohmann;
        json j;
        j["name"] = name;
        j["tweaks"]["creative-no-clip"] = data.creativeNoClip;
        j["tweaks"]["auto-tool"] = data.enableAutoTool;
        for (int i = 0; i < data.hud_config.size(); i++) {
            auto typeString = trapdoor::getStringFromHUDType(static_cast<HUDItemType>(i));
            if (typeString != "unknown") {
                j["hud"][typeString] = data.hud_config[i];
            }
        }

        const auto fileName = "./plugins/trapdoor/player/" + name + ".json";
        std::ofstream f(fileName);
        if (!f) {
            trapdoor::logger().warn("Can not write file {} to disk", fileName);
        }
        f << j.dump(4);
        f.close();
    }

    void UserConfig::setHUD(const string &name, int item, bool value) {
        auto &data = this->playerData[name];
        if (item < 0 || item >= data.hud_config.size()) {
            trapdoor::logger().error("Unknown error"); //理论上这里不会执行，以防万一还是加上
        }
        data.hud_config[item] = value;
        this->syncConfigToDisk(name);
    }

    void UserConfig::init() {
        namespace fs = std::filesystem;
        using namespace nlohmann;
        for (auto &f: fs::directory_iterator("./plugins/trapdoor/player")) {
            if (f.is_regular_file() && f.path().extension() == ".json") {


                auto playerName = f.path().stem().string();

                trapdoor::logger().debug("Read player[{}] 's config file ", playerName);

                std::ifstream file(f.path());
                if (!file.is_open()) {
                    trapdoor::logger().warn("Can not read player [{}]'s config file ", playerName);
                    continue;
                }

                try {
                    json j;
                    file >> j;
                    PlayerData data;

                    //读取tweak信息
                    data.creativeNoClip = j["tweaks"]["creative-no-clip"].get<bool>();
                    data.enableAutoTool = j["tweaks"]["auto-tool"].get<bool>();

                    //读取
                    auto &hud_cfg = j["hud"];

                    for (auto &[item, on]: hud_cfg.items()) {
                        auto type = getHUDTypeFromString(item);
                        if (type != Unknown) {
                            data.hud_config[static_cast<int>(type)] = on.get<bool>();
                        }
                    }
                    this->playerData[playerName] = data;
                } catch (std::exception &e) {
                    trapdoor::logger().warn("Can not read player [{}]'s config file: {}", f.path().stem().string(),
                                            e.what());
                }


            }

        }
        //读取配置文件
    }
}