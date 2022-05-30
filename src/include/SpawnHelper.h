#ifndef TRAPDOOR_SPAWN_HELPER_H
#define TRAPDOOR_SPAWN_HELPER_H
#include <MC/Player.hpp>

#include "CommandHelper.h"

namespace tr {
    ActionResult PrintCap();

    ActionResult CountActors(Player *p, const std::string &type);

    ActionResult printSpawnProbability(Player *player, const BlockPos &pos);

    //! 这个结构的代码不要动，顺序也别动

#pragma pack(push, 1)
    struct TSpawnConditions {
        /*0*/ bool isOnSurface = false;
        /*1*/ bool isInWater = false;
        /*2*/ bool land = false;
        /*3*/ bool isInLava = false;
        /*4*/ bool isUnderground = false;
        char gap2[11]{};
        /*16*/ uint32_t rawBrightness = 0;
        /*20*/ BlockPos pos{};
    };

    static_assert(offsetof(TSpawnConditions, isOnSurface) == 0);
    static_assert(offsetof(TSpawnConditions, isInWater) == 1);
    static_assert(offsetof(TSpawnConditions, isUnderground) == 4);
    static_assert(offsetof(TSpawnConditions, rawBrightness) == 16);
    static_assert(offsetof(TSpawnConditions, pos) == 20);
#pragma pack(pop)

    class SpawnHelper {
        std::vector<BlockPos> verticalSpawnPositions;
        bool enable = false;
        int dimensionID = 0;
        unsigned long long gameTick;

       public:
        inline void setAble(bool able) { this->enable = able; }

        inline bool isEnable() const { return this->enable; }

        void tick();

        void draw();

        void updateVerticalSpawnPositions(const BlockPos &pos, Player *player);
    };

}  // namespace tr
#endif