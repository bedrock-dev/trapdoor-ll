//
// Created by xhy on 2022/6/16.
//

#ifndef TRAPDOOR_SPAWNANALYZER_H
#define TRAPDOOR_SPAWNANALYZER_H
#include <string>
#include <unordered_map>

#include "CommandHelper.h"
#include "TBlockPos.h"
namespace trapdoor {
    class SpawnAnalyzer {
       public:
        static const int SAMPLING_RARE = 10;
        SpawnAnalyzer() = default;
        void AddMob(Mob* mob, const std::string& name, bool surface);
        void tick();
        ActionResult clear();
        ActionResult start(int id, const TBlockPos2& pos);
        ActionResult stop();
        [[nodiscard]] ActionResult printResult() const;

       private:
        void collectDensityInfo();
        bool inAnalyzing = false;
        int dimensionID = 0;
        TBlockPos2 centerChunkPos;
        size_t tick_count = 0;
        std::unordered_map<std::string, size_t> surfaceMobs;
        std::unordered_map<std::string, size_t> caveMobs;

        std::unordered_map<std::string, size_t> surfaceMobsPerTick;
        std::unordered_map<std::string, size_t> caveMobsPerTick;
    };

}  // namespace trapdoor

#endif  // TRAPDOOR_SPAWNANALYZER_H
