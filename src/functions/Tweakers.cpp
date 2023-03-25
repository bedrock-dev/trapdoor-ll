//
// Created by xhy on 2022/6/29.
//

#include "Tweakers.h"

#include <mc/Block.hpp>
#include <mc/BlockLegacy.hpp>
#include <mc/ItemStackBase.hpp>
#include <mc/PistonBlockActor.hpp>

#include "Configuration.h"
#include "HookAPI.h"
#include "TrapdoorMod.h"

/**
 * bool BlockSource::mayPlace(class Block const &, class BlockPos const &, unsigned char, class
Actor *, bool, class Vec3)
[21338400]?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_NVVec3@@@Z
*/
THook(bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_NVVec3@@@Z",
      void *bs, void *block, void *p, unsigned char face, void *placer, bool ignoreEntity,
      void *vec3) {
    auto level = trapdoor::mod().getConfig().getGlobalFunctionConfig().forcePlaceLevel;
    if (level == 1) return original(bs, block, p, face, placer, true, vec3);
    if (level == 2) return true;
    return original(bs, block, p, face, placer, ignoreEntity, vec3);
}

THook(bool, "?canOpen@ChestBlockActor@@QEBA_NAEAVBlockSource@@@Z", void *container, void *bs) {
    if (trapdoor::mod().getConfig().getGlobalFunctionConfig().forceOpenContainer) {
        return true;
    } else {
        return original(container, bs);
    }
}

THook(void, "?removeItem@Container@@UEAAXHH@Z", void *container, int slot, int count) {
    if (!trapdoor::mod().getConfig().getGlobalFunctionConfig().dropperNoCost) {
        original(container, slot, count);
    }
}

THook(void, "?explode@Explosion@@QEAAXXZ", void *self) {
    if (!trapdoor::mod().getConfig().getGlobalFunctionConfig().safeExplosion) {
        original(self);
    }
}

THook(void, "?updateNeighborsAt@BlockSource@@QEAAXAEBVBlockPos@@@Z", void *self, void *pos) {
    if (!trapdoor::mod().getConfig().getGlobalFunctionConfig().disableNCUpdate) {
        original(self, pos);
    }
}

THook(void, "?setPlayerGameType@ServerPlayer@@UEAAXW4GameType@@@Z", ServerPlayer *player,
      GameType mode) {
    original(player, mode);
    if (player && mode == GameType::GameTypeCreative &&
        trapdoor::mod().getConfig().getGlobalFunctionConfig().creativeNoClip &&
        trapdoor::mod().getUserConfig().noclip(player->getRealName())) {
        player->setAbility(static_cast<AbilitiesIndex>(17), true);
    }
}

// #include <mc/Player.hpp>
// THook(void,
//       "?_tickLevelChunksAroundActor@LevelChunkTickingSystem@@CAXAEAVActor@@AEAVBlockSource@@"
//       "AEAVLoadedChunksComponent@@@Z",
//       Actor *actor, BlockSource *bs, void *comp) {
//     if (actor) {
//         trapdoor::logger().debug("Actor name is {} {} {}",
//                                  reinterpret_cast<Player *>(actor)->getRealName(), (void *)bs,
//                                  comp);
//     } else {
//         trapdoor::logger().debug("Actor is null");
//     }
//
//     original(actor, bs, comp);
// }