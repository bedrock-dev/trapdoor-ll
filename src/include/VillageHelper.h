#ifndef TRAPDOOR_VILLAGE_HELPER_H
#define TRAPDOOR_VILLAGE_HELPER_H

#include <MC/Village.hpp>
#include <unordered_map>

#include "CommandHelper.h"

namespace tr {

    class VillageHelper {
       private:
        void SetVillagerHeadInfo();

       public:
        void HeavyTick();
        void LightTick();
        void InsertVillage(Village* village);
        // action
        ActionResult ListTickingVillages(bool details);

        ActionResult ShowBounds(bool able);

        ActionResult ShowCenter(bool able);

        ActionResult ShowPoiQury(bool able);

        ActionResult ShowIronSpawnArea(bool able);

        ActionResult ShowVillagerHeadInfo(bool able);

       private:
        std::unordered_map<int, Village*> vs_;
        bool show_center_ = false;
        bool show_iron_spawn_ = false;
        bool show_bounds_ = false;
        bool show_poi_query_ = false;
        bool show_head_info_ = false;
    };

}  // namespace tr

#endif
