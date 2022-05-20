#include "VillageHelper.h"

#include <MC/Dimension.hpp>
#include <MC/Level.hpp>
#include <MC/Village.hpp>
#include <MC/VillageManager.hpp>
#include <unordered_set>

#include "GlobalServiceAPI.h"
#include "TrapdoorMod.h"

namespace tr {

    namespace {

        std::unordered_set<Village*>& tickingVillages() {
            static std::unordered_set<Village*> vs;
            return vs;
        }
    }  // namespace

    // Actions
    ActionResult printTickingVillages() {
        for (auto v : tickingVillages()) {
            if (v) {
                // auto c = v->getCenter();
                // tr::logger().debug("uid: {} radius:{} center: [{} {} {}]",
                //                    v->getUniqueID().asString(),
                //                    v->getApproximateRadius(), c.x, c.y, c.z);
                tr::logger().debug("p: {}", reinterpret_cast<uint64_t>(v));
            }
        }
        return {"~", true};
    }
    ActionResult printNearestVillageInfo() { return {"~", true}; }

}  // namespace tr

THook(void, "?tick@Village@@QEAAXUTick@@AEAVBlockSource@@@Z", Village* village,
      void* tick, void* bs) {
    tr::tickingVillages().insert(village);
    original(village, tick, bs);
}