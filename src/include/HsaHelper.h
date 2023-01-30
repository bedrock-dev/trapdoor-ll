#ifndef TRAPDOOR_HSA_HELPER
#define TRAPDOOR_HSA_HELPER

#include <mc/LevelChunk.hpp>
#include <set>

#include "CommandHelper.h"
#include "TBlockPos.h"

namespace trapdoor {
    class HsaManager {
        bool showHsa = false;
        std::unordered_map<BlockPos, LevelChunk::HardcodedSpawningArea> globalAreas;

       public:
        void HeavyTick();

        ActionResult place(Player *player, bool replaceAirOnly);

        ActionResult count();

        inline ActionResult ShowHsa(bool show) {
            this->showHsa = show;
            return trapdoor::OperationSuccess();
        }
    };
}  // namespace trapdoor

#endif
