#ifndef TRAPDOOR_SPAWN_HELPER_H
#define TRAPDOOR_SPAWN_HELPER_H
#include <MC/Player.hpp>

#include "CommandHelper.h"

namespace tr {
    ActionResult PrintCap();

    ActionResult CountActors(Player *p, const std::string &type);

    ActionResult printSpawnProbability(Player *player, const BlockPos &pos);

    //! 这个结构的代码不要动，顺序也别动
    struct SpawnConditions {
        bool isOnSurface = false;
        bool isInWater = false;
        bool isInLava = false;
        bool isUnderground = false;
        uint32_t gap = 0;
        uint64_t delayEndWorldAge = 0;
        uint32_t rawBrightness = 0;
        BlockPos pos{};
    };

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