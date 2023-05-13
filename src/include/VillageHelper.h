#ifndef TRAPDOOR_VILLAGE_HELPER_H
#define TRAPDOOR_VILLAGE_HELPER_H

#include <mc/Village.hpp>
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

        // 原函数POIInstance::getOwnerCapacity
        inline uint64_t getPOIOwnerCapacity(POIInstance *poi){
            return dAccess<uint64_t>(poi,0xB0);
        }

        // 原函数POIInstance::getOwnerCount
        inline uint64_t getPOIOwnerCount(POIInstance *poi){
            return dAccess<uint64_t>(poi,0xA0);
        }

        // 原函数POIInstance::getWeight
        inline uint64_t getPOIWeight(POIInstance *poi){
            return dAccess<uint64_t>(poi,0xB8);
        }

        // 原函数POIInstance::getTypeName
        inline std::string getPOITypeName(POIInstance *poi){
            auto type = dAccess<int>(poi,0xC4);
            switch (type){
                case 0:
                    return "Bed";
                case 1:
                    return "MeetingArea";
                case 2:
                    return "Jobsite";
                case 3:
                    return "Count";
                default:
                    return "InvalidPOI";
            }
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
