#ifndef TRAPDOOR_HUD_HELPER_H
#define TRAPDOOR_HUD_HELPER_H
#include <array>
#include <unordered_map>

#include "CommandHelper.h"

namespace trapdoor {
    enum HUDInfoType {
        Unknown = 0,
        Base = 1,
        Mspt = 2,
        Vill = 3,
        Redstone = 4,
        Counter = 5,
        Chunk = 6,
        Cont = 7,  // 容器，写全名会和原版容器冲突
    };

    // 在上面增加子项目后记得修改下面的值，这个值似乎没有使用编译期指令获取
    constexpr auto HUD_TYPE_NUMBER = 8;

    struct PlayerHudInfo {
        std::string realName;
        // bool enable; 删除 HUD show true命令
        std::vector<int> config;
    };

    class HUDHelper {
       public:
        inline ActionResult setAble(bool able) {
            this->enable = able;  // 全局开关
            return {"Success", true};
        }

        void tick();

        ActionResult modifyPlayerInfo(const std::string& playerName, const std::string& item,
                                      int op);

        // ActionResult setAblePlayer(const std::string& playerName, bool able);

        static std::vector<std::string> getHUDItemStringList();

        void init();

       private:
        void tickChunk();
        bool readConfigCacheFromFile();
        bool syncConfigToFile();
        bool enable = true;
        std::unordered_map<std::string, PlayerHudInfo> playerInfos;
    };

}  // namespace trapdoor

#endif
