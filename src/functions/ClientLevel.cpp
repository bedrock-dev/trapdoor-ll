#include "ClientLevel.h"

#include <stdio.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "FMT/core.h"
#include "HookAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"

namespace trapdoor {
    namespace fs = std::filesystem;

    namespace {
        std::string getClientLevelRootPath() {
            std::string appdata = std::getenv("APPDATA");

            if (appdata.empty()) return "";

            fs::path p(appdata);

            p = p.parent_path();
            p /= "Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\LocalState\\games\\com."
                 "mojang\\"
                 "minecraftWorlds\\";
            return p.string();
        }
        std::vector<std::string> getALlLevels() {
            std::vector<std::string> res;

            auto root = getClientLevelRootPath();
            for (const auto &entry : fs::directory_iterator(root)) {
                auto levelname = entry.path() / "levelname.txt";

                if (fs::exists(levelname)) {
                    std::fstream f(levelname);
                    if (f.is_open()) {
                        std::string name;
                        f >> name;
                        f.close();
                        res.push_back(entry.path().filename().string() + "  " + name);
                    }
                }
            }
            return res;
        }

    }  // namespace

    void setupClientLevel() {
        if (!trapdoor::mod().getConfig().getBasicConfig().useClientLevel) return;
        trapdoor::logger().info("You have enable client mode!");
        trapdoor::logger().info("下面是所有的客户端存档，请选择一个！！");
        auto level = getALlLevels();

        int index = 1;
        for (auto l : level) {
            trapdoor::logger().debug("[" + std::to_string(index) + "]  " + l);
            index++;
        }
        // int x = 1;
        // scanf("%d", &x);
        // trapdoor::logger().info("{}", tb.get());
    }
}  // namespace trapdoor

// THook(std::string *,
//       "?getWorldsPath@FilePathManager@Core@@QEBA?AV?$PathBuffer@V?$basic_string@DU?$char_traits@D@"
//       "std@@V?$allocator@D@2@@std@@@2@XZ",
//       void *self, void *a, void *b) {
//     auto r = original(self, a, b);
//     // trapdoor::logger().debug("Root path is {}, appdata path is {}", *r,
//     //                          trapdoor::getClientLevelRootPath());
//     // // *r = std::string(
//     //
//     "C:/Users/xhy/AppData/Local/Packages/Microsoft.MinecraftUWP_8wekyb3d8bbwe/LocalState/games/"
//     //     "com.mojang/minecraftWorlds/");
//     return r;
// }
