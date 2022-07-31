#include <DynamicCommandAPI.h>

#include <MC/Container.hpp>
#include <MC/ItemStack.hpp>
#include <MC/SimpleContainer.hpp>
#include <MC/SimulatedPlayer.hpp>

#include "Msg.h"
#include "ScheduleAPI.h"
#include "SimPlayerHelper.h"
#include "TrapdoorMod.h"
namespace trapdoor {
    namespace {

#define GET_FREE_PLAYER(sim)                                       \
    auto*(sim) = this->tryFetchSimPlayer(name, true);              \
    if (!(sim)) {                                                  \
        return {"Player dost not exists or in scheduling", false}; \
    }

#define ADD_TASK                                            \
    task();                                                 \
    if (repType != 0) {                                     \
        auto sch = Schedule::repeat(task, interval, times); \
        this->simPlayers[name].task = sch;                  \
    }

#define CHECK_SURVIVAL                \
    if (!this->checkSurvival(name)) { \
        this->cancel(name);           \
        return;                       \
    }

        BlockPos getTargetPos(Player* p, BlockPos pos) {
            if (!p) return pos;
            auto* a = reinterpret_cast<Actor*>(p);
            auto ins = a->getBlockFromViewVector();
            if (!ins.isNull()) {
                return ins.getPosition();
            } else {
                return BlockPos::MAX;
            }
        }  // namespace

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

        void writeInvToFile(Container& cont, const std::string& playerName) {
            // const std::string path = "./plugins/trapdoor/sim/" + file_name;
            if (!trapdoor::mod().getConfig().getBasicConfig().keepSimPlayerInv) return;
            const std::string path =
                "./plugins/trapdoor/sim/" + std::to_string(do_hash(playerName.c_str()));
            trapdoor::logger().debug("Path is {}", path);
            // TODO 序列化背包内容到文件
            nlohmann::json obj;
            auto array = nlohmann::json::array();
            for (auto i = 0; i < cont.getSize(); i++) {
                auto* item = cont.getSlot(i);
                std::string sNbt;
                if (item) {
                    sNbt = item->getNbt()->toSNBT();
                }
                json j = {{"slot", i}, {"nbt", sNbt}};
                array.push_back(j);
            }
            obj["inventory"] = array;
            obj["name"] = playerName;
            std::ofstream f(path);
            if (!f) {
                trapdoor::logger().error("can not write file {} to disk", path);
                return;
            }

            f << obj;
            f.close();
        }

        void tryReadInvFromFile(Container& cont, const std::string& playerName) {
            if (!trapdoor::mod().getConfig().getBasicConfig().keepSimPlayerInv) return;
            const std::string path =
                "./plugins/trapdoor/sim/" + std::to_string(do_hash(playerName.c_str()));
            // TODO 从文件读取内容到背包
            std::ifstream f(path);
            if (!f.is_open()) {
                //  trapdoor::logger().warn("can not read file {}", file_name);
                return;
            }

            nlohmann::json obj;
            f >> obj;
            try {
                auto invInfo = obj["inventory"];
                for (auto& item : invInfo) {
                    auto slot = item["slot"].get<int>();
                    auto* it = ItemStack::create(CompoundTag::fromSNBT(item["nbt"]));
                    cont.setItem(slot, *it);
                }
            } catch (const std::exception&) {
            }
        }

        constexpr auto DEFAULT_FACING = static_cast<ScriptModuleMinecraft::ScriptFacing>(1);
    }  // namespace

    bool SimPlayerManager::checkSurvival(const std::string& name) {
        auto it = this->simPlayers.find(name);
        if (it == this->simPlayers.end()) return false;
        return it->second.simPlayer != nullptr;
    }

    void SimPlayerManager::cancel(const std::string& name) {
        trapdoor::logger().debug("cancel task of:{}", name);
        auto it = this->simPlayers.find(name);
        if (it != this->simPlayers.end()) {
            it->second.task.cancel();
        }
    }

    void SimPlayerManager::stopAction(const std::string& name) {
        auto* sim = this->tryFetchSimPlayer(name, false);
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

    SimulatedPlayer* SimPlayerManager::tryFetchSimPlayer(const std::string& name, bool needFree) {
        auto it = simPlayers.find(name);
        if (it == simPlayers.end()) {
            return nullptr;
        }
        auto& simInfo = it->second;
        if (!simInfo.simPlayer) return nullptr;
        if (needFree) return simInfo.task.isFinished() ? simInfo.simPlayer : nullptr;
        return simInfo.simPlayer;
    }

    ActionResult SimPlayerManager::getBackpack(const std::string& name, int slot) {
        GET_FREE_PLAYER(sim)
        TextBuilder builder;
        auto& inv = sim->getInventory();
        for (int i = 0; i < inv.getSize(); i++) {
            auto* itemStack = inv.getSlot(i);
            if (itemStack && (!itemStack->getName().empty())) {
                auto c = itemStack->getColor();
                builder.sText(TB::GRAY, " - ")
                    .textF("[%d] ", i)
                    .textF(" %s  ", itemStack->getName().c_str())
                    .text(" x ")
                    .num(itemStack->getCount())
                    .text("\n");
            }
        }
        return {builder.get(), true};
    }

    ActionResult SimPlayerManager::destroyOnSchedule(const std::string& name, const BlockPos& p,
                                                     Player* origin, int repType, int interval,
                                                     int times) {
        GET_FREE_PLAYER(sim)
        auto pos = p;
        if (pos == BlockPos::MAX) {
            pos = trapdoor::getLookAtPos(origin);
        }
        auto task = [name, this, sim, pos]() {
            CHECK_SURVIVAL
            if (pos != BlockPos::MAX) {
                sim->simulateDestroyBlock(pos, DEFAULT_FACING);
            }
        };
        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::destroySchedule(const std::string& name, int repType,
                                                   int interval, int times) {
        GET_FREE_PLAYER(sim)
        auto task = [name, this, sim]() {
            CHECK_SURVIVAL
            auto bi = sim->getBlockFromViewVector();
            if (bi.isNull()) {
                sim->simulateDestroy();
            } else {
                sim->simulateDestroyBlock(bi.getPosition(), DEFAULT_FACING);
            }
        };
        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::interactSchedule(const std::string& name, Player* origin,
                                                    int repType, int interval, int times) {
        if (!origin) {
            return ErrorPlayerNeed();
        }
        GET_FREE_PLAYER(sim)
        auto* playerActor = reinterpret_cast<Actor*>(origin);
        auto* target = playerActor->getActorFromViewVector(5.25);
        auto ins = playerActor->getBlockFromViewVector();
        auto pos = ins.isNull() ? BlockPos::MAX : ins.getPosition();
        auto task = [this, sim, name, pos, target]() {
            CHECK_SURVIVAL
            if (target) {
                sim->simulateInteract(*target);
            } else {
                if (pos == BlockPos::MAX) {
                    sim->simulateInteract();
                } else {
                    sim->simulateInteract(pos, DEFAULT_FACING);
                }
            }
        };
        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::attackSchedule(const std::string& name, Player* origin,
                                                  int repType, int interval, int times) {
        GET_FREE_PLAYER(sim)
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
            CHECK_SURVIVAL
            auto t = Global<Level>->fetchEntity(uid, true);
            if (t) {
                sim->simulateAttack(t);
            } else {
                sim->simulateAttack();
            }
        };
        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::useSchedule(const std::string& name, int itemId, int repType,
                                               int interval, int times) {
        GET_FREE_PLAYER(sim)
        auto task = [this, name, sim, itemId]() {
            CHECK_SURVIVAL
            auto* item = getItemInInv(sim, itemId);
            this->stopAction(name);
            if (item) {
                sim->simulateSetItem(*item, true, 0);
                sim->simulateUseItem(*item);
            } else {
                sim->simulateUseItem();
            }
        };
        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::jumpSchedule(const std::string& name, int repType, int interval,
                                                int times) {
        auto* sim = this->tryFetchSimPlayer(name, true);
        if (!sim) {
            return {"No player or player is in scheduling", false};
        }
        auto task = [this, name, sim]() {
            CHECK_SURVIVAL
            sim->simulateJump();
        };
        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::useOnBlockSchedule(const std::string& name, int itemId,
                                                      const BlockPos& p, Player* ori, int repType,
                                                      int interval, int times) {
        GET_FREE_PLAYER(sim)
        auto pos = getTargetPos(ori, p);
        auto task = [this, name, pos, itemId, sim]() {
            CHECK_SURVIVAL
            auto v = Vec3(0.5, 1.0, 0.5);
            auto* item = getItemInInv(sim, itemId);
            if (item) {
                sim->simulateUseItemOnBlock(*item, pos, DEFAULT_FACING, v);
            }
        };
        ADD_TASK
        return {"", true};
    }
    ActionResult SimPlayerManager::setItem(const string& name, int itemId) {
        GET_FREE_PLAYER(sim)
        auto* item = getItemInInv(sim, itemId);
        if (item) {
            sim->simulateSetItem(*item, true, 0);
        }
        return {"", true};
    }
    ActionResult SimPlayerManager::dropItem(const string& name, int itemId) {
        //        GET_FREE_PLAYER(sim)
        //        auto* item = getItemInInv(sim, itemId);
        //        if (item) {
        //            sim->_drop(*item, true);
        //        }
        //        return {"", true};
        return {"Developing", false};
    }
    ActionResult SimPlayerManager::behavior(const std::string& name, const std::string& behType,
                                            const Vec3& vec) {
        auto sim = this->tryFetchSimPlayer(name, false);
        if (!sim) {
            return {"player does not exist", false};
        }
        if (behType == "lookat") {
            sim->simulateLookAt(vec + Vec3(0.5, 0.5, 0.5));
        } else if (behType == "moveto") {
            sim->simulateMoveToLocation(vec + Vec3(0.5, 1.0, 0.5), 1.0f);
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
        this->refreshCommandSoftEnum();
        return {"", true};
    }

    ActionResult SimPlayerManager::addPlayer(const std::string& name, const BlockPos& p, int dimID,
                                             Player* origin) {
        auto iter = this->simPlayers.find(name);
        if (iter != simPlayers.end() && iter->second.simPlayer) {
            return {"Player has already existed", false};
        }
        if (iter != simPlayers.end()) {
            iter->second.task.cancel();
        }
        auto* sim = SimulatedPlayer::create(name, p, dimID);

        if (!sim) {
            return {"Spawn player failure", false};
        }
        if (origin) {
            // 是玩家召唤的
            auto rot = origin->getRotation();
            sim->teleport(origin->getPos() - Vec3(0.0f, 1.62f, 0.0f), dimID, rot.x, rot.y);
        }
        this->simPlayers[name] = {name, sim, ScheduleTask()};
        tryReadInvFromFile(sim->getInventory(), name);
        this->refreshCommandSoftEnum();
        return {"", true};
    }

    // 定时做垃圾回收，解决数据不同步问题
    void SimPlayerManager::tick() {}
    ActionResult SimPlayerManager::listAll() {
        if (this->simPlayers.empty()) {
            return {"No player exists", true};
        }
        TextBuilder builder;
        for (const auto& i : this->simPlayers) {
            builder.text(" - ").textF("%s   ", i.first.c_str());
            if (!i.second.simPlayer) {
                builder.sText(TB::RED | TB::BOLD, "Not exist\n");
            } else {
                if (i.second.task.isFinished()) {
                    builder.sText(TB::GREEN | TB::BOLD, "Free      ");
                } else {
                    builder.sText(TB::YELLOW | TB::BOLD, "Working   ");
                }
                auto pos = i.second.simPlayer->getPosition().toBlockPos();
                auto dim = i.second.simPlayer->getDimensionId();
                builder.textF("  %d @ [%d %d %d]\n", static_cast<int>(dim), pos.x, pos.y, pos.z);
            }
        }

        return {builder.get(), true};
    }
    void SimPlayerManager::processDieEvent(const string& name) {
        trapdoor::logger().debug("try disconnect {}", name);
        this->removePlayer(name);
    }
    void SimPlayerManager::refreshCommandSoftEnum() {
        if (!this->cmdInstance) return;
        std::vector<std::string> names;
        for (auto& sp : this->simPlayers) {
            names.push_back(sp.first);
        }
        cmdInstance->setSoftEnum("name", names);
    }
    void SimPlayerManager::tryRefreshInv(Player* player) {
        if (!player) return;
        auto name = player->getRealName();
        auto iter = this->simPlayers.find(name);
        if (iter == this->simPlayers.end()) return;
        // todo 序列化背包物品到文件
        writeInvToFile(player->getInventory(), name);
    }
}  // namespace trapdoor
