#include <DynamicCommandAPI.h>

#include <mc/ChunkSource.hpp>
#include <mc/Container.hpp>
#include <mc/Dimension.hpp>
#include <mc/GameRule.hpp>
#include <mc/GameRuleId.hpp>
#include <mc/GameRules.hpp>
#include <mc/ItemStack.hpp>
#include <mc/OwnerStorageEntity.hpp>
#include <mc/ServerNetworkHandler.hpp>
#include <mc/SimpleContainer.hpp>
#include <mc/SimulatedPlayer.hpp>
#include <mc/StackResultStorageEntity.hpp>

#include "CommandHelper.h"
#include "Msg.h"
#include "Nlohmann/json.hpp"
#include "ScheduleAPI.h"
#include "SimPlayerHelper.h"
#include "SimulateAPIWrapper.h"
#include "TrapdoorMod.h"
#include "Utils.h"

namespace trapdoor {
    namespace {

#define GET_FREE_PLAYER(sim)                             \
    auto *(sim) = this->tryFetchSimPlayer(name, true);   \
    if (!(sim)) {                                        \
        return ErrorMsg("player.error.schedule-failed"); \
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

        bool enableKeepInventory() {
            auto &rules = Global<Level>->getGameRules();
            auto *rule = rules.getRule(rules.nameToGameRuleIndex("keepinventory"));
            if (rule == nullptr) {
                return false;
            }
            return rule->getBool();
        }

        //
        //        BlockPos getTargetPos(Player *p, const BlockPos &pos) {
        //            if (!p) return pos;
        //            auto *a = reinterpret_cast<Actor *>(p);
        //            auto ins = a->getBlockFromViewVector();
        //            if (!ins.isNull()) {
        //                return ins.getPosition();
        //            } else {
        //                return BlockPos::MAX;
        //            }
        //        }  // namespace

        void writeEmptyInvToFile(const std::string &playerName) {
            trapdoor::logger().debug("Clear player {}' inventory due to death");
            const std::string path =
                trapdoor::mod().rootPath() + "/sim/" + std::to_string(do_hash(playerName.c_str()));
            namespace fs = std::filesystem;
            fs::remove(path);
        }

        constexpr auto DEFAULT_FACING = static_cast<ScriptModuleMinecraft::ScriptFacing>(1);

    }  // namespace

    bool SimPlayerManager::checkSurvival(const std::string &name) {
        auto it = this->simPlayers.find(name);
        if (it == this->simPlayers.end()) return false;
        return it->second.simPlayer != nullptr;
    }

    void SimPlayerManager::cancel(const std::string &name) {
        trapdoor::logger().debug("Cancel task of sim player {}", name);
        auto it = this->simPlayers.find(name);
        if (it != this->simPlayers.end()) {
            it->second.task.cancel();
            it->second.driver.stop();
        }
    }

    void SimPlayerManager::stopAction(const std::string &name) {
        auto *sim = this->tryFetchSimPlayer(name, false);
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

    SimulatedPlayer *SimPlayerManager::tryFetchSimPlayer(const std::string &name, bool needFree) {
        auto it = simPlayers.find(name);
        if (it == simPlayers.end()) {
            return nullptr;
        }
        auto &simInfo = it->second;
        if (!simInfo.simPlayer) return nullptr;
        // 既没有在调度，也没有在执行脚本
        if (needFree)
            return (simInfo.task.isFinished() && (!simInfo.driver.isRunning())) ? simInfo.simPlayer
                                                                                : nullptr;
        return simInfo.simPlayer;
    }

    ActionResult SimPlayerManager::getBackpack(const std::string &name, int slot) {
        auto it = this->simPlayers.find(name);
        if (it == this->simPlayers.end()) return trapdoor::ErrorMsg("player.error.not-exist");
        auto *sim = it->second.simPlayer;
        TextBuilder builder;
        size_t ctr = 0;
        auto &inv = sim->getInventory();
        for (int i = 0; i < inv.getSize(); i++) {
            auto *itemStack = inv.getSlot(i);
            if (itemStack && (!itemStack->getName().empty())) {
                ctr++;
                auto c = itemStack->getColor();
                builder.sText(TB::GRAY, " - ")
                    .textF("[%d] ", i)
                    .textF(" %s  ", itemStack->getName().c_str())
                    .text(" x ")
                    .num(itemStack->getCount())
                    .text("\n");
            }
        }
        if (ctr == 0) builder.text(tr("player.info.empty-inv"));
        return {builder.get(), true};
    }

    /**********************************destroy position /  destroy(lookat position)
     * ******************************************/
    // 挖掘指定坐标
    ActionResult SimPlayerManager::destroyPositionSchedule(const std::string &name,
                                                           const BlockPos &p, Player *origin,
                                                           int repType, int interval, int times) {
        GET_FREE_PLAYER(sim)
        auto pos = p;
        if (pos == BlockPos::MAX) {
            pos = trapdoor::getLookAtPos(origin);
        }

        auto task = [name, this, sim, pos]() {
            CHECK_SURVIVAL
            if (pos != BlockPos::MAX) {
                sim->simulateDestroyBlock(pos, DEFAULT_FACING);
                sim->sendInventory(true);
            }
        };
        ADD_TASK
        return {"", true};
    }

    // 挖掘假人看向的坐标
    ActionResult SimPlayerManager::destroySchedule(const std::string &name, int repType,
                                                   int interval, int times) {
        GET_FREE_PLAYER(sim)
        auto task = [name, this, sim]() {
            CHECK_SURVIVAL
            auto bi = sim->getBlockFromViewVector();
            if (!bi.isNull()) {
                sim->simulateDestroyBlock(bi.getPosition(), DEFAULT_FACING);
                sim->sendInventory(true);
            }
        };
        ADD_TASK
        return {"", true};
    }

    /******************************物品右键行为**********************************/

    ActionResult SimPlayerManager::useOnPositionSchedule(const std::string &name, int itemID,
                                                         const BlockPos &p, Player *origin,
                                                         int repType, int interval, int times) {
        GET_FREE_PLAYER(sim)
        auto pos = p;
        if (pos == BlockPos::MAX) {
            pos = trapdoor::getLookAtPos(origin);
        }

        auto task = [this, name, pos, itemID, sim]() {
            CHECK_SURVIVAL
            auto v = Vec3(0.5, 1.0, 0.5);
            /// 有位置信息 且背包内有对应的物品
            if (pos != BlockPos::MAX && bot::switchItemToHandById(sim, itemID)) {
                sim->simulateUseItemOnBlock(*bot::getSelectItem(sim), pos, DEFAULT_FACING, v);
                sim->sendInventory(true);
            }
        };
        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::useOnSchedule(const string &name, int itemID, Player *ori,
                                                 int repType, int interval, int times) {
        GET_FREE_PLAYER(sim)
        auto task = [name, this, sim, itemID]() {
            CHECK_SURVIVAL
            auto v = Vec3(0.5, 1.0, 0.5);
            auto bi = sim->getBlockFromViewVector();
            if (!(bi.isNull()) && bot::switchItemToHandById(sim, itemID)) {
                sim->simulateUseItemOnBlock(*bot::getSelectItem(sim), bi.getPosition(),
                                            DEFAULT_FACING, v);
                sim->sendInventory(true);
            }
        };
        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::useSchedule(const std::string &name, int itemId, int repType,
                                               int interval, int times) {
        GET_FREE_PLAYER(sim)
        auto task = [this, name, sim, itemId]() {
            CHECK_SURVIVAL
            if (bot::switchItemToHandById(sim, itemId)) {  // 交换然后使用就行了
                trapdoor::logger().debug("try use item {}", bot::getSelectItem(sim)->getName());
                sim->simulateUseItem(*bot::getSelectItem(sim));
                sim->sendInventory(true);
            }
        };
        ADD_TASK
        return {"", true};
    }
    /**************************************可实体交互的Schedule*****************************************************/
    // 右键行为
    ActionResult SimPlayerManager::interactSchedule(const std::string &name, Player *origin,
                                                    int repType, int interval, int times) {
        if (!origin) {
            return ErrorPlayerNeed();
        }
        GET_FREE_PLAYER(sim)
        auto *playerActor = reinterpret_cast<Actor *>(origin);
        auto *target = playerActor->getActorFromViewVector(5.25);
        auto ins = playerActor->getBlockFromViewVector();
        auto pos = ins.isNull() ? BlockPos::MAX : ins.getPosition();
        auto uid = ActorUniqueID();
        if (target) uid = target->getUniqueID();
        auto task = [this, sim, name, pos, uid]() {
            CHECK_SURVIVAL
            auto t = Global<Level>->fetchEntity(uid, true);
            if (t) {
                sim->simulateInteract();
            } else {
                if (pos == BlockPos::MAX) {
                    sim->simulateInteract();
                } else {
                    sim->simulateInteract(pos, DEFAULT_FACING);
                }
            }
            sim->sendInventory(true);
        };
        ADD_TASK
        return {"", true};
    }

    // 左键行为
    ActionResult SimPlayerManager::attackSchedule(const std::string &name, Player *origin,
                                                  int repType, int interval, int times) {
        GET_FREE_PLAYER(sim)
        Actor *target{nullptr};
        ActorUniqueID uid;
        if (origin) {
            auto *playerActor = reinterpret_cast<Actor *>(origin);
            target = playerActor->getActorFromViewVector(5.25);
            if (target) {
                uid = target->getUniqueID();
            }
        }

        auto task = [this, name, sim, uid]() {
            CHECK_SURVIVAL
            auto t = Global<Level>->fetchEntity(uid, true);
            if (t && t != sim) {
                sim->simulateAttack(t);
            } else {
                sim->simulateAttack();
            }

            sim->sendInventory(true);
        };
        ADD_TASK
        return {"", true};
    }
    /*********************************其他的Schedule*******************************/

    // 执行命令
    ActionResult SimPlayerManager::runCmdSchedule(const string &name, const string &command,
                                                  int repType, int interval, int times) {
        GET_FREE_PLAYER(sim)
        auto task = [name, this, sim, command]() {
            CHECK_SURVIVAL
            sim->runcmd(command);
            sim->sendInventory(true);
        };

        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::jumpSchedule(const std::string &name, int repType, int interval,
                                                int times) {
        GET_FREE_PLAYER(sim)
        auto task = [this, name, sim]() {
            CHECK_SURVIVAL
            sim->simulateJump();
            sim->sendInventory(true);
        };
        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::setItem(const string &name, int itemId, int slot) {
        GET_FREE_PLAYER(sim)
        if (slot == -1) {  // 等于-1，itemID有效，搜索背包(如果不是-1就表示就是直接的slot)
            bot::searchFirstItemInInvById(sim, slot, itemId);
        }
        bot::swapTwoSlotInInventory(sim->getInventory(), slot, sim->getSelectedItemSlot());
        sim->sendInventory(true);
        return {"", true};
    }

    ActionResult SimPlayerManager::dropItem(const string &name, int itemId) {
        // TODO drop item
        GET_FREE_PLAYER(sim)

        int slot = -1;
        auto *item = bot::searchFirstItemInInvById(sim, slot, itemId);
        if (item) {
            // 丢完物品后手动删除
            sim->drop(*item, false);
            sim->getInventory().removeItem(slot, 64);
            sim->sendInventory(true);
        }
        return {"", true};
    }

    /**
     * 丢出背包内的所有物品
     * @param name  假人名字
     * @param itemID  物品id
     * @return
     */
    ActionResult SimPlayerManager::dropAllItems(const string &name, int itemID) {
        GET_FREE_PLAYER(sim)
        auto &inv = sim->getInventory();
        int sz = inv.getSize();
        for (int i = 0; i < sz; i++) {
            auto *item = inv.getSlot(i);
            if (item && (itemID == INT_MAX || item->getId() == itemID)) {
                sim->drop(*item, false);
                sim->getInventory().removeItem(i, 64);
            }
        }

        sim->sendInventory(true);

        return {"", true};
    }

    ActionResult SimPlayerManager::behavior(const std::string &name, const std::string &behType,
                                            const Vec3 &vec) {
        GET_FREE_PLAYER(sim)
        if (behType == "lookat") {
            sim->simulateLookAt(vec + Vec3(0.5, 0.5, 0.5));
        } else if (behType == "moveto") {
            sim->simulateMoveToLocation(vec + Vec3(0.5, 1.0, 0.5), 1.0f);
        } else if (behType == "navto") {
            sim->simulateNavigateToLocation(vec + Vec3(0.5, 1.0, 0.5), 1.0f);
        }

        return {"", true};
    }

    ActionResult SimPlayerManager::removePlayer(const std::string &name) {
        auto it = this->simPlayers.find(name);
        if (it == this->simPlayers.end()) {
            return ErrorMsg("player.error.not-exists");
        }

        it->second.task.cancel();
        it->second.driver.stop();
        if (it->second.simPlayer) it->second.simPlayer->simulateDisconnect();
        simPlayers.erase(name);
        // this->syncPlayerListToFile();
        this->refreshCommandSoftEnum();
        return {"", true};
    }

    ActionResult SimPlayerManager::addPlayer(const std::string &name, const Vec3 &p, int dimID,
                                             int gameMode, Player *origin) {
        auto iter = this->simPlayers.find(name);
        if (iter != simPlayers.end() && iter->second.simPlayer) {
            // 玩家已经存在
            return ErrorMsg("player.error.existed");
        }

        auto *sim = SimulatedPlayer::create(name, p, AutomaticID<Dimension, int>(dimID));
        if (!sim) {
            return ErrorMsg("player.error.spawn");
        }
        if (origin) {
            // 是玩家召唤的
            auto rot = origin->getRotation();
            sim->teleport(origin->getPos() - Vec3(0.0f, 1.62001f, 0.0f), dimID, rot.x, rot.y);
        }

        sim->setPlayerGameType(static_cast<GameType>(gameMode));
        this->simPlayers[name] = {name, sim, ScheduleTask()};
        bot::tryReadInvFromFile(sim->getInventory(), name);
        sim->sendInventory(true);
        this->refreshCommandSoftEnum();
        return OperationSuccess();
    }

    void SimPlayerManager::tick() {
        for (auto &bot : this->simPlayers) {
            bot.second.driver.tick();
        }
    }

    ActionResult SimPlayerManager::listAll() {
        if (this->simPlayers.empty()) {
            return ErrorMsg("player.error.no-players");
        }

        TextBuilder builder;
        for (const auto &i : this->simPlayers) {
            builder.text(" - ").textF("%s   ", i.first.c_str());
            if (!i.second.simPlayer) {
                builder.sText(TB::RED | TB::BOLD, "Not exist\n");
            } else {
                if (i.second.task.isFinished() && (!i.second.driver.isRunning())) {
                    builder.sText(TB::GREEN | TB::BOLD, "Free      ");
                } else if (i.second.driver.isRunning()) {
                    builder.sText(TB::BLUE | TB::BOLD, "Running Script   ");
                } else {
                    builder.sText(TB::YELLOW | TB::BOLD, "Task scheduling   ");
                }
                auto pos = i.second.simPlayer->getPosition().toBlockPos();
                auto dim = i.second.simPlayer->getDimensionId();
                builder.textF("  %d @ [%d %d %d]\n", static_cast<int>(dim), pos.x, pos.y, pos.z);
            }
        }
        return {builder.get(), true};
    }

    void SimPlayerManager::processDieEvent(const string &name) {
        trapdoor::logger().debug("player {} try disconnect", name);
        auto iter = this->simPlayers.find(name);
        if (iter == this->simPlayers.end()) return;

        // 如果没开死亡不掉落就清空玩家背包
        if (!enableKeepInventory()) {
            writeEmptyInvToFile(name);
        }

        this->removePlayer(name);
    }

    void SimPlayerManager::refreshCommandSoftEnum() {
        if (!this->cmdInstance) return;
        std::vector<std::string> names;
        for (auto &sp : this->simPlayers) {
            names.push_back(sp.first);
        }
        cmdInstance->setSoftEnum("name", names);
    }

    void SimPlayerManager::refreshCommandScriptSoftEnum(const std::vector<std::string> &scripts) {
        trapdoor::logger().debug("Set soft enums");
        cmdInstance->setSoftEnum("file", scripts);
    }

    void SimPlayerManager::syncPlayerListToFile() {
        const std::string path = "./plugins/trapdoor/sim/cache.json";
        nlohmann::json obj;
        for (auto &sim : this->simPlayers) {
            auto *p = sim.second.simPlayer;
            if (p) {
                auto pos = p->getPos();
                nlohmann::json j = {{"x", pos.x},
                                    {"y", pos.y - 1},
                                    {"z", pos.z},
                                    {"dim", (int)p->getDimensionId()},
                                    {"mode", (int)p->getPlayerGameType()}

                };
                obj[sim.first] = j;
            }
        }

        std::ofstream f(path);
        if (!f.is_open()) {
            return;
        }
        f << obj.dump(4);
        f.close();
    }

    void SimPlayerManager::addPlayersInCache() {
        const std::string path = "./plugins/trapdoor/sim/cache.json";
        nlohmann::json obj;
        std::ifstream i(path);
        if (!i.is_open()) {
            return;
        }
        i >> obj;
        try {
            for (const auto &item : obj.items()) {
                const auto name = item.key();
                const auto &value = item.value();
                auto dim = value["dim"].get<int>();
                auto x = value["x"].get<float>();
                auto y = value["y"].get<float>();
                auto z = value["z"].get<float>();
                auto mode = value["mode"].get<int>();
                this->addPlayer(name, {x, y, z}, dim, mode, nullptr);
                trapdoor::logger().warn("Spawn sim player [{}] at {},{},{} in dim {}", name, x, y,
                                        z, dim);
                //    tempConfig.enable = value["enable"].get<bool>();
            }
        } catch (const std::exception &e) {
            trapdoor::logger().error("error read sim player cache: {}", e.what());
        }

        i.close();
    }

    void SimPlayerManager::savePlayerInventoryToFile() {
        for (auto &kv : this->simPlayers) {
            if (kv.second.simPlayer) {
                bot::writeInvToFile(kv.second.simPlayer->getInventory(), kv.first);
            }
        }
    }

    ActionResult SimPlayerManager::followActor(const std::string &name, Player *player) {
        if (!player) {
            return ErrorPlayerNeed();
        }
        GET_FREE_PLAYER(sim)

        auto *playerActor = reinterpret_cast<Actor *>(player);
        auto uid = playerActor->getUniqueID();
        auto *target = playerActor->getActorFromViewVector(5.25);
        if (target) uid = target->getUniqueID();
        if (uid == sim->getUniqueID()) return ErrorMsg("player.error.follow");
        auto task = [this, sim, name, uid]() {
            CHECK_SURVIVAL
            auto t = Global<Level>->fetchEntity(uid, true);
            if (t) {
                sim->simulateNavigateToEntity(*t, 4.3f);
            }
        };
        int repType = 1;
        int interval = 20;
        int times = -1;
        ADD_TASK
        return {"", true};
    }

    ActionResult SimPlayerManager::getSimPlayerInfo(const string &name) {
        GET_FREE_PLAYER(sim)
        trapdoor::TextBuilder builder;
        builder.textF("- Name: %s\n", sim->getName().c_str())
            .textF("- Xuid: %s\n", sim->getXuid().c_str())
            .textF("- Uid: %ld\n", sim->getUniqueID().get())
            .textF("- Game mode: %d\n", static_cast<int>(sim->getPlayerGameType()))
            .textF("- Command Permission level: %d\n",
                   static_cast<int>(sim->getCommandPermissionLevel()))
            .textF("- Input speed: %.2f\n", sim->_getInputSpeed())
            .textF("- Spawn chunk limit: %d\n", sim->_getSpawnChunkLimit())
            .textF("- Has owned chunk source: %d\n", sim->hasOwnedChunkSource())
            .textF("- Chunk radius: %d\n", sim->getChunkRadius())
            .textF("- Block source %p\n", &sim->getRegion())
            .textF("- Has level: %d", sim->hasLevel());
        // .textF("- Has",sim->_updateChunkPublisherView())
        return {builder.get(), true};
    }

    ActionResult SimPlayerManager::teleportTo(const string &name, const Vec3 &position) {
        return ErrorDeveloping();
        //        GET_FREE_PLAYER(sim)
        //        sim->teleportTo(position, true, 0, 0, true);
        //        return {"", false};
        //
    }

    ActionResult SimPlayerManager::swapBackpack(const string &name, Player *origin) {
        if (!origin) return ErrorPlayerNeed();
        GET_FREE_PLAYER(sim)
        auto *actor = origin->getActorFromViewVector(5.25);

        // 创造模式不需要指向
        if ((!origin->isCreative()) && (actor != sim)) {
            return trapdoor::ErrorMsg("player.error.point-to-needed");
        }

        if (origin->getInventorySize() != sim->getInventorySize()) {
            return trapdoor::ErrorUnexpected("Invalid backpack size");
        }

        auto &simInv = sim->getInventory();
        auto &playerInv = origin->getInventory();
        // 备份玩家数据
        auto size = playerInv.getSize();

        for (auto i = 0; i < size; i++) {
            auto iSim = simInv.getItem(i).clone();
            auto iPlayer = playerInv.getItem(i).clone();
            playerInv.removeItem(i, 64);
            simInv.removeItem(i, 64);
            playerInv.setItem(i, iSim);
            simInv.setItem(i, iPlayer);
        }

        sim->sendInventory(true);
        origin->sendInventory(true);
        return OperationSuccess();
    }

    ActionResult SimPlayerManager::runScript(const std::string &name, const std::string &scriptName,
                                             int interval, bool stopWhenError) {
        auto path = trapdoor::mod().rootPath() + "/scripts/" + scriptName;
        auto it = this->simPlayers.find(name);
        if (it == this->simPlayers.end() || !(it->second.task.isFinished()) ||
            it->second.driver.isRunning()) {
            return trapdoor::ErrorMsg("player.error.schedule-failed");
        }

        auto &driver = it->second.driver;
        if (!driver.init(path, it->second.simPlayer, interval, stopWhenError)) {
            return trapdoor::ErrorMsg("Init Script Engine failure, please check the script");
        }
        return trapdoor::OperationSuccess();
    }

}  // namespace trapdoor
/*
 * 定时保存备背包数据(同步)
 */
THook(void, "?save@Level@@UEAAXXZ", Level *self) {
    trapdoor::mod().getSimPlayerManager().savePlayerInventoryToFile();
    original(self);
}