#ifndef TRAPDOOR_HUD_HELPER_H
#define TRAPDOOR_HUD_HELPER_H

#include <array>
#include <unordered_map>

#include "CommandHelper.h"

namespace trapdoor {
    enum HUDItemType {
        Unknown = 0,
        Base = 1,
        Mspt = 2,
        Vill = 3,
        Redstone = 4,
        Counter = 5,
        Chunk = 6,
        Cont = 7,  // 容器，写全名会和原版容器冲突
        //如果有新的项目请按照这个顺序往后写


        //用于记录长度，没有其他意思
        LEN,
    };

    HUDItemType getHUDTypeFromString(const std::string &str);

    std::string getStringFromHUDType(HUDItemType t);

    class HUDHelper {
    public:

        inline ActionResult setAble(bool able) {
            this->enable = able;  // 全局开关
            return trapdoor::OperationSuccess();
        }


        void tick();

    public :
        static ActionResult modifyPlayerInfo(const std::string &playerName, const std::string &item,
                                             int op);

        static std::vector<std::string> getHUDItemStringList();

    private:
        void tickChunk() const;

        //  bool readConfigCacheFromFile();

        // bool syncConfigToFile();

        bool volatile enable = true;
        //   std::unordered_map<std::string, PlayerHudInfo> playerInfos;
    };


}  // namespace trapdoor

#endif
