#include <MC/ItemStack.hpp>
#include <MC/SimpleContainer.hpp>
#include <MC/SimulatedPlayer.hpp>

#include "Msg.h"
#include "ScheduleAPI.h"
#include "SimPlayerHelper.h"
#include "TrapdoorMod.h"

namespace tr {

    namespace {

        BlockPos getTargetPos(Player* p, BlockPos pos) {
            if (!p) return pos;
            auto* a = reinterpret_cast<Actor*>(p);
            auto ins = a->getBlockFromViewVector();
            if (!ins.isNull()) {
                return ins.getPosition();
            } else {
                return tr::INVALID_POS;
            }
        }

        ItemStack* getItemInInv(SimulatedPlayer* sim, int itemID) {
            if (!sim) return nullptr;
            auto& inv = sim->getInventory();
            int sz = inv.getSize();
            for (int i = 0; i < sz; i++) {
                auto* item = inv.getSlot(i);
                if (item && item->getId() == itemID) {
                    return item;
                }
            }
            return nullptr;
        }

    }  // namespace

    bool SimPlayerManager::checkSurvival(const std::string& name) {
        return this->players.find(name) != this->players.end();
    }

    void SimPlayerManager::cancel(const std::string& name) {
        tr::logger().debug("cancel task of:{}", name);
        auto it = this->taskList.find(name);
        if (it != this->taskList.end()) {
            it->second.cancel();
            taskList.erase(it);
        }
    }

    void SimPlayerManager::stopAction(const std::string& name) {
        auto* sim = this->tryFetchFreeSimPlayer(name);
        if (sim) {
            sim->simulateStopUsingItem();
            sim->simulateStopMoving();
            sim->simulateStopInteracting();
            sim->simulateStopDestroyingBlock();
            sim->stopDestroying();
            sim->stopRiding(true, true, true);
            sim->stopSleepInBed(true, true);
            sim->stopUsingItem();
        }
    }

    SimulatedPlayer* SimPlayerManager::tryFetchFreeSimPlayer(const std::string& name) {
        auto it = simPlayers.find(name);
        if (it == simPlayers.end()) {
            return nullptr;
        }
        auto& simInfo = it->second;
        if (simInfo.simPlayer && simInfo.task.cancel()) {
            return simInfo.simPlayer;
        }

        //        auto iter = this->players.find(name);
        //        if (iter == this->players.end()) {
        //            return nullptr;
        //        }
        //        auto it2 = this->taskList.find(name);
        //        // 没有任务
        //        if (it2 == this->taskList.end()) {
        //            return iter->second;
        //        } else {
        //            return nullptr;
        //        }
    }

    ActionResult SimPlayerManager::getBackpack(const std::string& name, int slot) {
        auto iter = this->players.find(name);
        if (iter == this->players.end()) {
            return {"Player does not exist", false};
        }
        auto& p = iter->second;
        TextBuilder builder;
        auto& inv = p->getInventory();
        for (int i = 0; i < inv.getSize(); i++) {
            auto* itemStack = inv.getSlot(i);
            if (itemStack && itemStack->getCount() != 0) {
                builder.textF("[%d] => %s * %d\n", i, itemStack->getName().c_str(),
                              itemStack->getCount());
            }
        }
        return {builder.get(), true};
    }

    ActionResult SimPlayerManager::destroySchedule(const std::string& name, const BlockPos& p,
                                                   Player* origin, int repType, int interval,
                                                   int times) {
        auto* sim = this->tryFetchFreeSimPlayer(name);
        if (!sim) {
            return {"No player or player is in scheduling", false};
        }

        auto pos = p;
        if (origin) {
            auto* a = reinterpret_cast<Actor*>(origin);
            auto ins = a->getBlockFromViewVector();
            if (!ins.isNull()) {
                pos = ins.getPosition();
            }
        }

        auto task = [name, this, sim, pos]() {
            if (!this->checkSurvival(name)) this->cancel(name);
            if (pos == INVALID_POS) {
                sim->simulateDestory();
            } else {
                sim->simulateDestroyBlock(pos, static_cast<ScriptFacing>(1));
            }
        };

        if (repType == 0) {
            task();
        } else {
            auto sch = Schedule::repeat(task, interval, times);
            this->taskList[name] = sch;
        }
        return {"", true};
    }

    ActionResult SimPlayerManager::interactSchedule(const std::string& name, Player* origin,
                                                    int repType, int interval, int times) {
        auto* sim = this->tryFetchFreeSimPlayer(name);
        if (!sim) {
            return {"No player or player is in scheduling", false};
        }
        // 这边有个潜在的空指针bug
        auto* playerActor = reinterpret_cast<Actor*>(origin);
        auto* target = playerActor->getActorFromViewVector(5.25);
        auto ins = playerActor->getBlockFromViewVector();
        auto pos = ins.isNull() ? tr::INVALID_POS : ins.getPosition();
        auto task = [this, sim, name, pos, target]() {
            if (!this->checkSurvival(name)) this->cancel(name);
            if (target) {
                sim->simulateInteract(*target);
            } else {
                if (pos == INVALID_POS) {
                    sim->simulateInteract();
                } else {
                    sim->simulateInteract(pos, static_cast<ScriptFacing>(1));
                }
            }
        };
        task();
        if (repType != 0) {
            auto sch = Schedule::repeat(task, interval, times);
            this->taskList[name] = sch;
        }
        return {"", true};
    }

    ActionResult SimPlayerManager::attackSchedule(const std::string& name, Player* origin,
                                                  int repType, int interval, int times) {
        auto* sim = this->tryFetchFreeSimPlayer(name);
        if (!sim) {
            return {"No player or player is in scheduling", false};
        }

        Actor* target = nullptr;
        ActorUniqueID uid;
        if (origin) {
            auto* playerActor = reinterpret_cast<Actor*>(origin);
            target = playerActor->getActorFromViewVector(5.25);
            if (target) {
                uid = target->getUniqueID();
            }
        }

        auto task = [this, name, sim, uid]() {
            if (!this->checkSurvival(name)) this->cancel(name);
            auto t = Global<Level>->fetchEntity(uid, true);
            if (t) {
                sim->simulateAttack(t);
            } else {
                sim->simulateAttack();
            }
        };

        task();
        if (repType != 0) {
            auto sch = Schedule::repeat(task, interval, times);
            this->taskList[name] = sch;
        }
        return {"", true};
    }

    ActionResult SimPlayerManager::useSchedule(const std::string& name, int itemId, int repType,
                                               int interval, int times) {
        auto* sim = this->tryFetchFreeSimPlayer(name);
        if (!sim) {
            return {"No player or player is in scheduling", false};
        }
        auto task = [this, name, sim, itemId]() {
            if (!this->checkSurvival(name)) this->cancel(name);
            auto* item = getItemInInv(sim, itemId);
            if (item) {
                sim->simulateUseItem(*item);
            } else {
                sim->simulateUseItem();
            }
        };
        task();
        if (repType != 0) {
            auto sch = Schedule::repeat(task, interval, times);
            this->taskList[name] = sch;
        }
        return {"", true};
    }

    ActionResult SimPlayerManager::jumpSchedule(const std::string& name, int repType, int interval,
                                                int times) {
        auto* sim = this->tryFetchFreeSimPlayer(name);
        if (!sim) {
            return {"No player or player is in scheduling", false};
        }
        auto task = [this, name, sim]() {
            if (!this->checkSurvival(name)) this->cancel(name);
            sim->simulateJump();
        };
        task();
        if (repType != 0) {
            auto sch = Schedule::repeat(task, interval, times);
            this->taskList[name] = sch;
        }
        return {"", true};
    }

    ActionResult SimPlayerManager::useOnBlockSchedule(const std::string& name, int itemId,
                                                      const BlockPos& p, Player* ori, int repType,
                                                      int interval, int times) {
        auto* sim = this->tryFetchFreeSimPlayer(name);
        if (!sim) {
            return {"No player or player is in scheduling", false};
        }
        auto pos = getTargetPos(ori, p);
        auto task = [this, name, pos, itemId, sim]() {
            if (!this->checkSurvival(name)) this->cancel(name);
            auto face = static_cast<ScriptFacing>(1);
            auto v = Vec3(0.5, 1.0, 0.5);
            auto* item = getItemInInv(sim, itemId);
            if (item) {
                sim->simulateUseItemOnBlock(*item, pos, face, v);
            }
        };
        task();
        if (repType != 0) {
            auto sch = Schedule::repeat(task, interval, times);
            this->taskList[name] = sch;
        }
        return {"", true};
    }

    ActionResult SimPlayerManager::behavior(const std::string& name, const std::string& behType,
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
        auto it = this->simPlayers.find(name);
        if (it == this->simPlayers.end()) {
            return {"player does not exist", false};
        }
        it->second.task.cancel();
        if (it->second.simPlayer) it->second.simPlayer->simulateDisconnect();
        simPlayers.erase(name);
        return {"", true};
    }

    ActionResult SimPlayerManager::addPlayer(const std::string& name, const BlockPos& p,
                                             int dimID) {
        auto iter = this->simPlayers.find(name);
        if (iter != simPlayers.end() && iter->second.simPlayer) {
            return {"player has already existed", false};
        }

        if (iter != simPlayers.end()) {
            iter->second.task.cancel();
        }
        auto* sim = SimulatedPlayer::create(name, p, dimID);
        if (!sim) {
            return {"Spawn player failure", false};
        }
        this->simPlayers[name] = {name, sim, ScheduleTask()};
        return {"", true};
    }

    // 定时做垃圾回收，解决数据不同步问题
    void SimPlayerManager::tick() {}
    ActionResult SimPlayerManager::listAll() { return {"todo", true}; }
}  // namespace tr
