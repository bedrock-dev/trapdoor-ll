#ifndef TRAPDOOR_VILLAHE_H
#define TRAPDOOR_VILLAGE_H

#include "CommandHelper.h"

namespace tr {

    class VillageHelper {
       public:
        void TickParticles();

        ActionResult ListTickingVillages();

       private:
        static std::unordered_set<Village*> vs_;
        bool show_center_ = false;
        bool show_iron_spawn_ = false;
        bool show_bounds_ = false;
        bool show_poi_query_ = false;
        bool show_bounds_ = false;
    };

}  // namespace tr

#endif
