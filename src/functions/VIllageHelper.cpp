#include "VillageHelper.h"

#include <MC/Dimension.hpp>
#include <MC/Level.hpp>
#include <MC/Village.hpp>
#include <MC/VillageManager.hpp>
#include <unordered_set>

#include "GlobalServiceAPI.h"
#include "TrapdoorMod.h"

namespace tr {

    void VillageHelper::TickParticles() {}
    ActionResult VillageHelper::ListTickingVillages() {
        std::string res;
        for (auto v : this->vs_) {
            if (v) {
                auto c = v->getCenter();
                res += fmt::format("uid: {} radius:{} center: [{} {} {}]\n",
                                   v->getUniqueID().asString().substr(0, 5),
                                   v->getApproximateRadius(), c.x, c.y, c.z);
            }
        }
        return {res, true};
    }

}  // namespace tr

THook(void, "?tick@Village@@QEAAXUTick@@AEAVBlockSource@@@Z", Village* village,
      void* tick, void* bs) {
    //  tr::tickingVillages().insert(village);
    original(village, tick, bs);
}