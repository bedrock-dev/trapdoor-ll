#include <MC/ItemStack.hpp>
#include <MC/SimpleContainer.hpp>
#include <MC/SimulatedPlayer.hpp>

#include "Msg.h"
#include "SimPlayerHelper.h"
#include "TrapdoorMod.h"

namespace tr {

    ActionResult SimPlayerManager::useItemOnBlock(const std::string& name,
                                                  int slot, const BlockPos& pos,
                                                  Player* ori) {
        auto iter = this->players.find(name);
        if (iter == this->players.end()) {
            return {"player does not exist", false};
        }
        auto& p = iter->second;
        if (slot < 0 || slot >= p->getInventory().getSize()) {
            return {"invalid slot number", false};
        }

        if (!ori) {
            p->simulateUseItemInSlotOnBlock(
                slot, pos, static_cast<ScriptFacing>(1), Vec3(0.5, 1.0, 0.5));

            return {"", true};
        } else {
            auto* a = reinterpret_cast<Actor*>(ori);
            auto ins = a->getBlockFromViewVector();
            if (ins.isNull()) {
                return {"no pos found", false};
            } else {
                p->simulateUseItemInSlotOnBlock(slot, ins.getPosition(),
                                                static_cast<ScriptFacing>(1),
                                                Vec3(0.5, 1.0, 0.5));
            }

            return {"", true};
        }
    }
    ActionResult SimPlayerManager::useItemOnSlot(const std::string& name,
                                                 int slot) {
        auto iter = this->players.find(name);
        if (iter == this->players.end()) {
            return {"player does not exist", false};
        }
        auto& p = iter->second;
        if (slot < 0 || slot >= p->getInventory().getSize()) {
            return {"invalid slot number", false};
        }

        p->simulateUseItemInSlot(slot);
        return {"", true};
    }
    ActionResult SimPlayerManager::getBagpack(const std::string& name,
                                              int slot) {
        auto iter = this->players.find(name);
        if (iter == this->players.end()) {
            return {"player does not exist", false};
        }
        auto& p = iter->second;
        TextBuilder builder;

        auto& inv = p->getInventory();
        for (int i = 0; i < inv.getSize(); i++) {
            auto* itemStack = inv.getSlot(i);
            if (itemStack && itemStack->getCount() != 0) {
                builder.textF("[%d] => %s * %d\n", i,
                              itemStack->getName().c_str(),
                              itemStack->getCount());
            }
        }
        // tr::logger().debug("inv size:  {}", p->getInventory().getSize());
        // tr::logger().debug("hands size: {}",
        // p->getHandContainer().getSize());
        return {builder.get(), true};
    }

    ActionResult SimPlayerManager::destroy(const std::string& name,
                                           const BlockPos& pos,
                                           Player* origin) {
        auto iter = this->players.find(name);
        if (iter == this->players.end()) {
            return {"player does not exist", false};
        }
        auto& p = iter->second;
        if (!origin) {
            auto r = p->simulateDestroyBlock(pos, static_cast<ScriptFacing>(1));
            tr::logger().debug("1 = {}", r);
            return {"", true};
        }
        tr::logger().debug("test!");
        auto* a = reinterpret_cast<Actor*>(origin);
        auto ins = a->getBlockFromViewVector();
        if (ins.isNull()) {
            p->simulateDestory();
        } else {
            auto t = p->simulateDestroyBlock(ins.getPosition(),
                                             static_cast<ScriptFacing>(1));
            tr::logger().debug("2 = {}", t);
        }

        return {"", true};
    }

    ActionResult SimPlayerManager::interact(const std::string& name,
                                            Player* origin) {
        auto iter = this->players.find(name);
        if (iter == this->players.end()) {
            return {"player does not exist", false};
        }
        auto& p = iter->second;
        auto* a = reinterpret_cast<Actor*>(origin);
        auto* target = a->getActorFromViewVector(5.25);
        if (target) {
            p->simulateInteract(*target);

        } else {
            auto ins = a->getBlockFromViewVector();
            if (ins.isNull()) {
                p->simulateInteract();
            } else {
                p->simulateInteract(ins.getPosition(),
                                    static_cast<ScriptFacing>(0));
            }
        }
        return {"", true};
    }

    ActionResult SimPlayerManager::behavior(const std::string& name,
                                            const std::string& behType,
                                            const Vec3& vec) {
        auto iter = this->players.find(name);
        if (iter == this->players.end()) {
            return {"player does not exist", false};
        }
        auto& p = iter->second;
        if (behType == "lookat") {
            p->simulateLookAt(vec + Vec3(0.5, 0.5, 0.5));
        } else if (behType == "moveto") {
            p->simulateMoveToLocation(vec + Vec3(0.5, 1.0, 0.5), 1.0f);
        }

        return {"", true};
    }
    ActionResult SimPlayerManager::removePlayer(const std::string& name) {
        auto iter = this->players.find(name);
        if (iter == this->players.end()) {
            return {"player does not exist", false};
        }
        auto& p = iter->second;
        p->simulateDisconnect();

        this->players.erase(iter);
        return {"", true};
    }

    ActionResult SimPlayerManager::actionPlayer(const std::string& name,
                                                const std::string& action,
                                                const std::string& type,
                                                int extraAgrs) {
        tr::logger().debug("player: {} action:{}, type:{}, extea args{}", name,
                           action, type, extraAgrs);

        return {"", true};
    }
    ActionResult SimPlayerManager::addPlayer(const std::string& name,
                                             const BlockPos& p, int dimID) {
        auto iter = this->players.find(name);
        if (iter != players.end() && iter->second != nullptr) {
            return {"player has already existed", false};
        }
        auto* sim = SimulatedPlayer::create(name, p, dimID);
        if (!sim) {
            return {"spawn player failure", false};
        }
        this->players[name] = sim;
        return {"", true};
    }

    ActionResult SimPlayerManager::addTask(const std::string& name,
                                           SimTaskType type, const Vec3* vec3,
                                           Actor* target, int interval,
                                           int repeatTime) {
        return {"", true};
    }
}  // namespace tr