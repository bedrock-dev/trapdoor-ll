#ifndef TRAPDOOR_MINIHUD_HELPER_H
#define TRAPDOOR_MINIHUD_HELPER_H
#include <array>
#include <unordered_map>

#include "CommandHelper.h"

namespace tr {
    enum HUDInfoType {
        Base = 0,
        Mspt = 1,
        Vill = 2,
        Counter = 3,
        Unknown = 4,
    };

    struct PlayerHudInfo {
        std::string realName;
        bool enbale;
        std::array<int, 4> config{};
    };

    class HUDHelper {
       public:
        void tick();

        ActionResult modifyPlayerInfo(const std::string& playerName,
                                      const std::string& item, int op);

        ActionResult setAblePlayer(const std::string& playerName, bool able);

       private:
        bool enable = true;
        std::unordered_map<std::string, PlayerHudInfo> playerInfos;
    };

}  // namespace tr

#endif
