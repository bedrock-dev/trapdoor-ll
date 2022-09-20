#ifndef TRAPDOOR_VILLAGE_HELPER_H
#define TRAPDOOR_VILLAGE_HELPER_H

#include <MC/Village.hpp>
#include <unordered_map>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"

namespace trapdoor {
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

        ActionResult printDetails(const std::string& id, const Vec3& pos);

        void refreshCommandSoftEnum();
        //  ActionResult Goto(int vid, const Vec3& pos);

        std::string getVillagerInfo(Actor* v, bool highlight);

        ActionResult showVillageInfo(Player* p);

        inline void setupCommandInstance(const DynamicCommandInstance* cmd) {
            this->cmdInstance = cmd;
        }

       private:
        std::map<int, Village*> vs_;
        bool showCenter = false;
        bool showIronSpawn = false;
        bool showBounds = false;
        bool showPoiQuery = false;
        bool showHeadInfo = false;
        const DynamicCommandInstance* cmdInstance = nullptr;
    };

}  // namespace trapdoor

#endif
