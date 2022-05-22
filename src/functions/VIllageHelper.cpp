#include "VillageHelper.h"

#include <MC/Dimension.hpp>
#include <MC/Level.hpp>
#include <MC/Village.hpp>
#include <MC/VillageManager.hpp>
#include <unordered_set>

#include "DataConverter.h"
#include "GlobalServiceAPI.h"
#include "Particle.h"
#include "TrapdoorMod.h"

namespace tr {

    namespace {

        int getUIDFormPool(const std::string& uid) {
            int next_id = 1;
            static std::unordered_map<std::string, int> uid_pool;
            auto it = uid_pool.find(uid);
            if (it != uid_pool.end()) {
                return it->second;
            } else {
                uid_pool[uid] = next_id;
                next_id++;
                return next_id - 1;
            }
        }
    }  // namespace

    void VillageHelper::HeavyTick() {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 40;
        if (refresh_time != 0) return;
        tr::logger().debug("heavy tick!");
        for (auto v : this->vs_) {
            tr::logger().debug("draw bounds");
            auto aabb = tr::fromAABB(&(v->getBounds()));
            tr::DrawAABB(*aabb, tr::PCOLOR::RED, false, 0);
        }
    }

    void VillageHelper::LightTick() {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 20;
        if (refresh_time == 0) {
            this->vs_.clear();
        }
    }

    void VillageHelper::InsertVillage(Village* village) {
        if (village) this->vs_.insert(village);
    }

    ActionResult VillageHelper::ListTickingVillages(bool details) {
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

    ActionResult VillageHelper::ShowBounds(bool able) {
        this->show_bounds_ = able;
        return {"", true};
    }
}  // namespace tr

THook(void, "?tick@Village@@QEAAXUTick@@AEAVBlockSource@@@Z", Village* village,
      void* tick, void* bs) {
    tr::mod().village_helper().InsertVillage(village);
    original(village, tick, bs);
}