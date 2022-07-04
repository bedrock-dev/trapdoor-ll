#ifndef TRAPDOOR_VILLAGE_HELPER_H
#define TRAPDOOR_VILLAGE_HELPER_H

#include <MC/Village.hpp>
#include <unordered_map>

#include "CommandHelper.h"

namespace tr {

    class VillageHelper {
       private:
        void setVillagerHeadInfo();

       public:
        void heavyTick();
        void lightTick();
        void insertVillage(Village* village);
        // action
        ActionResult listTickingVillages(bool details);

        ActionResult setShowBounds(bool able);

        ActionResult setShowCenter(bool able);

        ActionResult setShowPoiQuery(bool able);

        ActionResult setShowIronSpawnArea(bool able);

        ActionResult setShowVillagerHeadInfo(bool able);

        ActionResult printDetails(int vid, const Vec3& pos);

        //  ActionResult Goto(int vid, const Vec3& pos);

        bool ShowVillageInfo(Player* p, Actor* actor);

       private:
        std::map<int, Village*> vs_;
        bool showCenter = false;
        bool showIronSpawn = false;
        bool showBounds = false;
        bool showPoiQuery = false;
        bool showHeadInfo = false;
    };

}  // namespace tr

#endif
