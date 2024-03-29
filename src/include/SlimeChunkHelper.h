
#ifndef TRAPDOOR_SLIMECHUNKHELPER_H
#define TRAPDOOR_SLIMECHUNKHELPER_H

#include <set>

#include "CommandHelper.h"
#include "TBlockPos.h"

namespace trapdoor {

    class SlimeChunkHelper {
        int showRadius = 5;
        bool showSlime = false;
        std::set<trapdoor::TBlockPos2> posList;

       public:
        inline ActionResult ShowSlime(bool show) {
            this->showSlime = show;
            return {"Slime chunk display is set to " + std::to_string(show), true};
        }

        void updateChunkPosList();

        void HeavyTick();

        ActionResult setRadius(int r);

        ActionResult draw();
    };
}  // namespace trapdoor

#endif  // TRAPDOOR_SLIMECHUNKHELPER_H
