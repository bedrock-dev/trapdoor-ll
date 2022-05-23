// clang-format off
//#include "GlobalServiceAPI.h"
#include "Global.h"
// clang-format on

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
#include "Msg.h"
#include "Particle.h"
#include "TAABB.h"
#include "TActor.h"
#include "TBlockPos.h"
#include "TrapdoorMod.h"

namespace tr {

    namespace {
        enum DwellerType { Villager = 0, IronGolem = 1, Cat = 2, XXX = 3 };

        // from Village::_trySpawnDefenderDwellers
        constexpr size_t DWELLER_POI_MAP_OFFSET = 96;
        // Village::_getDwellerMap
        constexpr size_t DWELLER_TICK_MAP_OFFSET = 160;

        typedef std::array<std::unordered_map<ActorUniqueID, tr::TDwellerData,
                                              TActorUniqueIDHash>,
                           4>
            DwellerTickMapType;

        //村庄的POI分配表
        typedef std::unordered_map<ActorUniqueID,
                                   std::array<std::weak_ptr<POIInstance>, 3>,
                                   TActorUniqueIDHash>
            DwellerPOIMapType;

        DwellerPOIMapType &Village_getDwellerPOIMap(Village *v) {
            return dAccess<DwellerPOIMapType, DWELLER_POI_MAP_OFFSET>(v);
        }

        DwellerTickMapType &Village_getDwellerTickMap(Village *v) {
            return dAccess<DwellerTickMapType, DWELLER_TICK_MAP_OFFSET>(v);
        }

        std::array<size_t, 4> Village_getDwellerCount(Village *v) {
            auto &map = Village_getDwellerTickMap(v);
            return {map[0].size(), map[1].size(), map[2].size(), map[3].size()};
        }

        void Village_showHeadInfo(int vid, Village *v) {
            auto map = Village_getDwellerPOIMap(v);
            int idx = 1;
            const char *icons[3] = {"B", "M", "J"};
            for (auto &kv : map) {
                auto actor = Global<Level>->fetchEntity(kv.first, false);
                if (actor) {
                    TextBuilder builder;
                    builder.textF("[%d] %d ", vid, idx);
                    ++idx;
                    for (int index = 0; index < 3; ++index) {
                        auto poi = kv.second[index].lock();
                        if (poi) {
                            builder.sTextF(TextBuilder::GREEN, "%s",
                                           icons[index]);
                        } else {
                            builder.sTextF(TextBuilder::RED, " %s",
                                           icons[index]);
                        }
                    }
                    actor->setNameTag(builder.get());
                }
            }

            auto tickMap = Village_getDwellerTickMap(v);
            int index = 0;
            for (auto i : tickMap) {
                for (auto &kv : i) {
                    auto actor = Global<Level>->fetchEntity(kv.first, false);
                    if (actor) {
                        if (index == DwellerType::Villager) {
                            actor->setNameTag(actor->getNameTag() + " " +
                                              std::to_string(kv.second.tick));
                        } else {
                            actor->setNameTag(std::to_string(kv.second.tick));
                        }
                    }
                }
                index++;
            }
        }

        int getVIDFormPool(const std::string &uid) {
            static int next_id = 1;
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

        if (this->show_head_info_) {
            this->SetVillagerHeadInfo();
        }
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

    void VillageHelper::SetVillagerHeadInfo() {
        for (auto &kv : this->vs_) {
            Village_showHeadInfo(kv.first, kv.second);
        }
    }
    ActionResult VillageHelper::ListTickingVillages(bool details) {
        if (this->vs_.empty()) {
            return {"no village in ticking", true};
        }

        tr::TextBuilder builder;
        for (auto kv : this->vs_) {
            if (kv.second) {
                auto v = kv.second;
                auto bound = fromAABB(v->getBounds());
                auto dc_map = Village_getDwellerCount(v);
                builder.text(" - ")
                    .sTextF(TextBuilder::GREEN, "[%d] ", kv.first)
                    .pos(tr::fromVec3(v->getCenter()).toBlockPos())
                    .text(" r:")
                    .num(v->getApproximateRadius())
                    .text(" p:")
                    .num(dc_map[static_cast<size_t>(DwellerType::Villager)])
                    .text(" g:")
                    .num(dc_map[static_cast<size_t>(DwellerType::IronGolem)])
                    .text(" b:")
                    .num(v->getBedPOICount())
                    .text(" [")
                    .pos(bound.p1.toBlockPos())
                    .text("")
                    .pos(bound.p2.toBlockPos())
                    .text("]\n");
            }
        }
        return {builder.get(), true};
    }

    ActionResult VillageHelper::PrintDetails(int vid, const Vec3 &pos) {
        if (this->vs_.empty()) {
            return {"no village", false};
        }

        if (vid == -1) {
            vid = this->vs_.begin()->first;
            auto dis = pos.distanceTo(this->vs_.begin()->second->getCenter());
            for (auto &kv : this->vs_) {
                auto newDis = pos.distanceTo(kv.second->getCenter());
                if (dis > newDis) {
                    dis = newDis;
                    vid = kv.first;
                }
            }
        }
        auto it = this->vs_.find(vid);
        if (it == this->vs_.end()) {
            return {"invalid index", false};
        }
        auto vill = it->second;

        TextBuilder builder;
        auto center = vill->getCenter().toBlockPos();
        auto minPos = vill->getBounds().pointA.toBlockPos();
        auto maxPos = vill->getBounds().pointB.toBlockPos();
        auto dc_map = Village_getDwellerCount(vill);
        builder
            .textF("VID: %d          UUID: %s", it->first,
                   vill->getUniqueID().asString().c_str())
            .text("\n- Center: ")
            .pos(fromBlockPos(center))
            .text("\n- Bounds: ")
            .pos(fromBlockPos(minPos))
            .text(" , ")
            .pos(fromBlockPos(maxPos))
            .text("\n")
            .text("- Radius: ")
            .num(vill->getApproximateRadius())
            .text("\n")
            .text("Dwellers: ")
            .sTextF(TextBuilder::GREEN, "%d / %d/ %d\n",
                    dc_map[static_cast<size_t>(DwellerType::Villager)],
                    dc_map[static_cast<size_t>(DwellerType::IronGolem)],
                    dc_map[static_cast<size_t>(DwellerType::Cat)])
            .text(
                "POIS:\n      Bed               |                  Work       "
                "|\n");
        auto map = Village_getDwellerPOIMap(vill);
        bool existAlarm = false;
        for (auto &villager : map) {
            for (int index = 0; index < 3; ++index) {
                if (index == 0) {
                    builder.text("|");
                }
                auto poi = villager.second[index].lock();
                if (index == 1) {
                    if (poi) existAlarm = true;
                    continue;
                }
                if (poi) {
                    auto pos = poi->getPosition();
                    auto cap = poi->getOwnerCapacity();
                    auto own = poi->getOwnerCount();
                    auto radius = poi->getRadius();
                    auto weight = poi->getWeight();

                    builder
                        .sTextF(TextBuilder::WHITE,
                                " [%d, %d, %d] %d/%d, %.1f, %lld ", pos.x,
                                pos.y, pos.z, own, cap, radius, weight)
                        .text("|");

                } else {
                    builder
                        .sText(TextBuilder::GRAY,
                               "            (null)             ")
                        .text("|");
                }

                if (index == 2) {
                    builder.text("\n");
                }
            }
        }
        builder.textF("Alarm:  %d", existAlarm);
        return {builder.get(), true};
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

    ActionResult VillageHelper::ShowVillagerHeadInfo(bool able) {
        this->show_head_info_ = true;
        return {"", true};
    }

}  // namespace tr

THook(void, "?tick@Village@@QEAAXUTick@@AEAVBlockSource@@@Z", Village *village,
      void *tick, void *bs) {
    tr::mod().village_helper().InsertVillage(village);
    original(village, tick, bs);
}