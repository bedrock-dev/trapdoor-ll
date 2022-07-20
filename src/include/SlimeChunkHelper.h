
#ifndef TRAPDOOR_SLIMECHUNKHELPER_H
#define TRAPDOOR_SLIMECHUNKHELPER_H

#include <set>

#include "CommandHelper.h"
#include "TBlockPos.h"

namespace trapdoor {

    class SlimeChunkHelper {
        int showRadius = 5;
        bool showSlime = false;
        std::set<trapdoor::ChunkPos> posList;

       public:
        inline ActionResult ShowSlime(bool show) {
            this->showSlime = show;
            return {"~", true};
        }

        void updateChunkPosList();

        void HeavyTick();

        ActionResult setRadius(int r);

        ActionResult draw();
    };
}  // namespace trapdoor

#endif  // TRAPDOOR_SLIMECHUNKHELPER_H
