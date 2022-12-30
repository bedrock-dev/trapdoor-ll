// clang-format off
#include "Global.h"
// clang-format on

#include "VillageHelper.h"

#include <array>
#include <mc/Dimension.hpp>
#include <mc/Level.hpp>
#include <mc/POIInstance.hpp>
#include <mc/Vec3.hpp>
#include <mc/Village.hpp>
#include <mc/VillageManager.hpp>
#include <unordered_set>

#include "DataConverter.h"
#include "Msg.h"
#include "Particle.h"
#include "TAABB.h"
#include "TActor.h"
#include "TBlockPos.h"
#include "TrapdoorMod.h"

namespace trapdoor {
    namespace {
        enum DwellerType { Villager = 0, IronGolem = 1, Cat = 2, XXX = 3 };

        // from Village::_trySpawnDefenderDwellers
        constexpr size_t DWELLER_POI_MAP_OFFSET = 96;
        // Village::_getDwellerMap
        constexpr size_t DWELLER_TICK_MAP_OFFSET = 160;

        typedef std::array<
            std::unordered_map<ActorUniqueID, trapdoor::TDwellerData, TActorUniqueIDHash>, 4>
            DwellerTickMapType;

        // 村庄的POI分配表
        typedef std::unordered_map<ActorUniqueID, std::array<std::weak_ptr<POIInstance>, 3>,
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
                            builder.sTextF(TextBuilder::GREEN, " %s", icons[index]);
                        } else {
                            builder.sTextF(TextBuilder::RED, " %s", icons[index]);
                        }
                    }
                    actor->setNameTag(builder.get());
                }
            }

            auto tickMap = Village_getDwellerTickMap(v);
            int index = 0;
            for (const auto &i : tickMap) {
                for (auto &kv : i) {
                    auto ac = Global<Level>->fetchEntity(kv.first, false);
                    if (ac) {
                        if (index == DwellerType::Villager) {
                            ac->setNameTag(ac->getNameTag() + "\n" +
                                           std::to_string(kv.second.tick));
                        } else {
                            ac->setNameTag(std::to_string(kv.second.tick));
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

    void VillageHelper::heavyTick() {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 40;
        if (refresh_time != 1) return;

        if (this->showHeadInfo) {
            this->setVillagerHeadInfo();
        }
        for (auto kv : this->vs_) {
            auto v = kv.second;
            if (this->showBounds) {
                trapdoor::drawAABB(fromAABB(v->getBounds()), trapdoor::PCOLOR::RED, false, 0);
            }

            if (this->showIronSpawn) {
                trapdoor::drawAABB(getIronSpawnArea(v->getCenter()), trapdoor::PCOLOR::VATBLUE,
                                   false, 0);
            }
            if (this->showCenter) {
                trapdoor::spawnParticle(fromVec3(v->getCenter()) + TVec3(0.5f, 0.9f, 0.5f),
                                        "minecraft:heart_particle", 0);
            }
            if (this->showPoiQuery) {
                trapdoor::drawAABB(getPOIQueryRange(fromAABB(v->getBounds())),
                                   trapdoor::PCOLOR::INDIGO, false, 0);
            }
        }
    }

    void VillageHelper::lightTick() {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 20;
        if (refresh_time == 0) {
            this->refreshCommandSoftEnum();
            this->vs_.clear();
        }
    }

    void VillageHelper::insertVillage(Village *village) {
        if (!village) return;
        auto uid = village->getUniqueID().asString();
        auto vid = getVIDFormPool(uid);
        this->vs_.insert({vid, village});
    }

    void VillageHelper::setVillagerHeadInfo() {
        for (auto &kv : this->vs_) {
            Village_showHeadInfo(kv.first, kv.second);
        }
    }
    ActionResult VillageHelper::listTickingVillages(bool details) {
        if (this->vs_.empty()) {
            return {"No village in ticking", true};
        }

        trapdoor::TextBuilder builder;
        for (auto kv : this->vs_) {
            if (kv.second) {
                auto v = kv.second;
                auto bound = fromAABB(v->getBounds());
                auto dc_map = Village_getDwellerCount(v);
                builder.text(" - ")
                    .sTextF(TextBuilder::GREEN, "[%d] ", kv.first)
                    .pos(trapdoor::fromVec3(v->getCenter()).toBlockPos())
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

    ActionResult VillageHelper::printDetails(const std::string &id, const Vec3 &pos) {
        trapdoor::logger().debug("Vid is {}", id);
        if (this->vs_.empty()) {
            return {"No village exists", false};
        }

        int targetVid = -1;
        // if id is empty,choose the closest one
        if (id.empty()) {
            targetVid = this->vs_.begin()->first;
            auto dis = pos.distanceTo(this->vs_.begin()->second->getCenter());
            for (auto &kv : this->vs_) {
                auto newDis = pos.distanceTo(kv.second->getCenter());
                if (dis > newDis) {
                    dis = newDis;
                    targetVid = kv.first;
                }
            }
        } else {
            // else compare id or uuid / choose id
            for (auto &kv : this->vs_) {
                if (kv.second) {
                    if (id.size() == 11) {  //"e1234567..."
                        if (kv.second->getUniqueID().asString().find(id.substr(0, 8)) == 0) {
                            targetVid = kv.first;
                            break;
                        }
                    } else {
                        if (id == std::to_string(kv.first)) {
                            targetVid = kv.first;
                            break;
                        }
                    }
                }
            }
        }

        auto it = this->vs_.find(targetVid);
        if (it == this->vs_.end()) {
            return {fmt::format("Invalid id: {}", id), false};
        }

        auto vill = it->second;

        TextBuilder builder;
        auto center = vill->getCenter().toBlockPos();
        auto minPos = vill->getBounds().min.toBlockPos();
        auto maxPos = vill->getBounds().max.toBlockPos();
        auto dc_map = Village_getDwellerCount(vill);
        builder
            .textF("VID: %d          UUID: %s", it->first, vill->getUniqueID().asString().c_str())
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
            .sTextF(TextBuilder::GREEN, "%d / %d / %d\n",
                    dc_map[static_cast<size_t>(DwellerType::Villager)],
                    dc_map[static_cast<size_t>(DwellerType::IronGolem)],
                    dc_map[static_cast<size_t>(DwellerType::Cat)])
            .text(
                "POIS:\n      Bed               |         Alarm          |     "
                "              Work         |\n");
        auto map = Village_getDwellerPOIMap(vill);
        [[maybe_unused]] bool existAlarm = false;
        for (auto &villager : map) {
            for (int index = 0; index < 3; ++index) {
                if (index == 0) {
                    builder.text("|");
                }
                auto poi = villager.second[index].lock();
                if (poi) {
                    auto position = poi->getPosition();
                    auto cap = poi->getOwnerCapacity();
                    auto own = poi->getOwnerCount();
                    auto radius = poi->getRadius();
                    auto weight = poi->getWeight();

                    builder
                        .sTextF(TextBuilder::WHITE, " [%d, %d, %d] %d/%d, %.1f, %lld ", position.x,
                                position.y, position.z, own, cap, radius, weight)
                        .text("|");
                } else {
                    builder.sText(TextBuilder::GRAY, "            (null)             ").text("|");
                }

                if (index == 2) {
                    builder.text("\n");
                }
            }
        }
        return {builder.get(), true};
    }

    ActionResult VillageHelper::setShowBounds(bool able) {
        this->showBounds = able;
        return {"", true};
    }

    ActionResult VillageHelper::setShowCenter(bool able) {
        this->showCenter = able;
        return {"", true};
    }

    ActionResult VillageHelper::setShowPoiQuery(bool able) {
        this->showPoiQuery = able;
        return {"", true};
    }

    ActionResult VillageHelper::setShowIronSpawnArea(bool able) {
        this->showIronSpawn = able;
        return {"", true};
    }

    ActionResult VillageHelper::setShowVillagerHeadInfo(bool able) {
        this->showHeadInfo = true;
        return {"", true};
    }

    std::string VillageHelper::getVillagerInfo(Actor *actor, bool highlight) {
        if (!actor) {
            return "";
        }
        auto aUid = actor->getUniqueID();
        TextBuilder builder;
        for (auto v : this->vs_) {
            auto dweller_map = Village_getDwellerPOIMap(v.second);
            auto iter = dweller_map.find(aUid);

            const static std::string name[] = {"Bed", "Alarm", "Work"};
            if (iter != dweller_map.end()) {
                builder.textF("VID: %d\n", v.first);
                for (int i = 0; i < 3; i++) {
                    const auto &poi = iter->second[i].lock();
                    if (poi) {
                        builder.textF("%s: [%d %d %d], %d/%d  %.2f  %zu\n",
                                      poi->getTypeName().c_str(), poi->getPosition().x,
                                      poi->getPosition().y, poi->getPosition().z,
                                      poi->getOwnerCount(), poi->getOwnerCapacity(),
                                      poi->getRadius(), poi->getWeight());
                        if (highlight) {
                            trapdoor::shortHighlightBlock(fromBlockPos(poi->getPosition()),
                                                          PCOLOR::TEAL, 0);
                        }
                    }
                }
            }
        }
        return builder.get();
    }

    ActionResult VillageHelper::showVillageInfo(Player *p) {
        if (!p) return trapdoor::ErrorPlayerNeed();
        auto actor = p->getActorFromViewVector(5.25);
        auto msg = this->getVillagerInfo(actor, true);
        if (msg.empty()) {
            return {"Can not find a villager", false};
        } else {
            return {msg, true};
        }
    }

    void VillageHelper::refreshCommandSoftEnum() {
        if (!this->cmdInstance) return;
        std::vector<std::string> ids;
        std::vector<std::string> uuids;
        for (auto &v : this->vs_) {
            if (v.second) {
                ids.push_back(std::to_string(v.first));
                uuids.push_back(v.second->getUniqueID().asString().substr(0, 8) + "...");
            }
        }
        ids.insert(ids.end(), uuids.begin(), uuids.end());
        cmdInstance->setSoftEnum("vid", ids);
    }

}  // namespace trapdoor

THook(void, "?tick@Village@@QEAAXUTick@@AEAVBlockSource@@@Z", Village *village, void *tick,
      void *bs) {
    trapdoor::mod().getVillageHelper().insertVillage(village);
    original(village, tick, bs);
}