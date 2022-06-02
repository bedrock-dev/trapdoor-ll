#ifndef TRAPDOOR_MINIHUD_HELPER_H
#define TRAPDOOR_MINIHUD_HELPER_H
#include <array>
#include <unordered_map>

namespace tr {
    enum HUDInfoType {
        Base = 0,
        Mspt = 1,
        Counter = 2,
    };

    struct PlayerHudInfo {
        std::string realName;
        bool enbale;
        std::array<HUDInfoType, 3> config{};
    };
    class HUDHelper {
       public:
        void tick();

       private:
        bool enable = true;
        std::unordered_map<std::string, PlayerHudInfo> playerInfos;
    };
}  // namespace tr

#endif
