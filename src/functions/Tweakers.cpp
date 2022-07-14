//
// Created by xhy on 2022/6/29.
//

#include "Tweakers.h"

#include <MC/PistonBlockActor.hpp>

#include "Config.h"
#include "HookAPI.h"
#include "TrapdoorMod.h"
THook(bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z", void *bs,
      void *block, void *p, unsigned char face, void *placer, bool ignoreEntity) {
    auto level = trapdoor::mod().getConfig().getTweakConfig().forcePlaceLevel;
    if (level == 1) return original(bs, block, p, face, placer, true);
    if (level == 2) return true;
    return original(bs, block, p, face, placer, ignoreEntity);
}
THook(bool, "?canOpen@ChestBlockActor@@QEBA_NAEAVBlockSource@@@Z", void *container, void *bs) {
    if (trapdoor::mod().getConfig().getTweakConfig().forceOpenContainer) {
        return true;
    } else {
        return original(container, bs);
    }
}

THook(void, "?removeItem@Container@@UEAAXHH@Z", void *container, int slot, int count) {
    if (!trapdoor::mod().getConfig().getTweakConfig().dropperNoCost) {
        original(container, slot, count);
    }
}
