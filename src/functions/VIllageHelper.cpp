#include "VillageHelper.h"

#include <MC/Dimension.hpp>
#include <MC/Level.hpp>
#include <MC/POIInstance.hpp>
#include <MC/Vec3.hpp>
#include <MC/Village.hpp>
#include <MC/VillageManager.hpp>
#include <array>
#include <unordered_set>

#include "DataConverter.h"
#include "GlobalServiceAPI.h"
#include "Msg.h"
#include "Particle.h"
#include "TAABB.h"
#include "TActor.h"
#include "TBlockPos.h"
#include "TrapdoorMod.h"

namespace tr {

    namespace {

        // from Village::_trySpawnDefenderDwellers
        constexpr size_t DWELLER_POI_MAP_OFFSET = 96;
        // Village::_getDwellerMap
        constexpr size_t DWELLER_TICK_MAP_OFFSET = 160;

        typedef std::array<std::unordered_map<ActorUniqueID, TDwellerData>, 4>
            DwellerTickMapType;

        typedef std::unordered_map<TActorUniqueID,
                                   std::array<std::weak_ptr<POIInstance>, 3>>
            DwellerPOIMapType;

        DwellerPOIMapType &Village_getDwellerPOIMap(Village *v) {
            return dAccess<DwellerPOIMapType, DWELLER_POI_MAP_OFFSET>(v);
        }

        DwellerTickMapType &Village_getDwellerTickMap(Village *v) {
            return dAccess<DwellerTickMapType, DWELLER_TICK_MAP_OFFSET>(v);
        }

        int getVIDFormPool(const std::string &uid) {
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

        TAABB getIronSpawnArea(const Vec3 &center) {
            auto vc = fromVec3(center);
            return {vc - TVec3(8, 6, 8), vc + TVec3(9, 7, 9)};
        }

        TAABB getPOIQueryRange(const TAABB &bound) {
            return {bound.p1 - TVec3(64, 64, 64), bound.p2 + TVec3(64, 64, 64)};
        }

    }  // namespace

    void VillageHelper::HeavyTick() {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 40;
        if (refresh_time != 1) return;
        for (auto kv : this->vs_) {
            auto v = kv.second;
            if (this->show_bounds_) {
                tr::DrawAABB(fromAABB(v->getBounds()), tr::PCOLOR::RED, false,
                             0);
            }
            if (this->show_iron_spawn_) {
                tr::DrawAABB(getIronSpawnArea(v->getCenter()), tr::PCOLOR::BLUE,
                             false, 0);
            }
            if (this->show_center_) {
                tr::SpawnParticle(
                    fromVec3(v->getCenter()) + TVec3(0.5f, 0.9f, 0.5f),
                    "minecraft:heart_particle", 0);
            }

            if (this->show_poi_query_) {
                tr::DrawAABB(getPOIQueryRange(fromAABB(v->getBounds())),
                             tr::PCOLOR::BLUE, false, 0);
            }
        }
    }

    void VillageHelper::LightTick() {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 20;
        if (refresh_time == 0) {
            this->vs_.clear();
        }
    }

    void VillageHelper::InsertVillage(Village *village) {
        if (!village) return;
        auto uid = village->getUniqueID().asString();
        auto vid = getVIDFormPool(uid);
        this->vs_.insert({vid, village});
    }

    ActionResult VillageHelper::ListTickingVillages(bool details) {
        std::string res;
        if (this->vs_.empty()) {
            return {"no village in ticking", true};
        }

        tr::TextBuilder builder;
        for (auto kv : this->vs_) {
            if (kv.second) {
                auto v = kv.second;
                auto bound = fromAABB(v->getBounds());
                builder.text(" - ")
                    .sTextF(TextBuilder::GREEN, "[%-2d]", kv.first)
                    .pos(tr::fromVec3(v->getCenter()).toBlockPos())
                    .text(" r:")
                    .num(v->getApproximateRadius())
                    .text(" p:")
                    .num(-1)
                    .text("/")
                    .num(-1)
                    .text(" g:")
                    .num(-1)
                    .text(" b:")
                    .num(v->getBedPOICount())
                    .text(" [")
                    .pos(bound.p1.toBlockPos())
                    .text("")
                    .pos(bound.p2.toBlockPos())
                    .text("]\n");
            }
        }
        return {res, true};
    }

    ActionResult VillageHelper::ShowBounds(bool able) {
        this->show_bounds_ = able;
        return {"", true};
    }

    ActionResult VillageHelper::ShowCenter(bool able) {
        this->show_center_ = able;
        return {"", true};
    }

    ActionResult VillageHelper::ShowPoiQury(bool able) {
        this->show_poi_query_ = able;
        return {"", true};
    }
    ActionResult VillageHelper::ShowIronSpawnArea(bool able) {
        this->show_iron_spawn_ = able;
        return {"", true};
    }
}  // namespace tr

THook(void, "?tick@Village@@QEAAXUTick@@AEAVBlockSource@@@Z", Village *village,
      void *tick, void *bs) {
    tr::mod().village_helper().InsertVillage(village);
    original(village, tick, bs);
}