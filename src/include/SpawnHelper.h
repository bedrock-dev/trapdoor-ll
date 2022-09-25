#ifndef TRAPDOOR_SPAWN_HELPER_H
#define TRAPDOOR_SPAWN_HELPER_H
#include <mc/ActorDefinitionIdentifier.hpp>
#include <mc/Player.hpp>

#include "CommandHelper.h"

namespace trapdoor {
    //! 这个结构的代码不要动，顺序也别动

    struct TMobSpawnRules {};
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

    ActionResult printCap(const ActorDefinitionIdentifier *id);

    ActionResult countActors(Player *p, const std::string &type);

    ActionResult printSpawnProbability(Player *player, const BlockPos &pos);

    ActionResult forceSpawn(Player *p, const ActorDefinitionIdentifier *id, const BlockPos &pos);

}  // namespace trapdoor
#endif