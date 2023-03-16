//
// Created by xhy on 2023/3/10.
//

#include "ScriptDriver.h"
#include <mc/Block.hpp>
#include <mc/BlockSource.hpp>
#include <mc/Brightness.hpp>
#include <mc/Dimension.hpp>
#include <mc/Level.hpp>
#include "TrapdoorMod.h"
#include <mc/BlockInstance.hpp>
#include <mc/Actor.hpp>
#include <mc/AttributeInstance.hpp>
#include "SimulateAPIWrapper.h"

namespace trapdoor {

    bool ScriptDriver::init(const string &fileName, SimulatedPlayer *p, int inter, bool err) {
        trapdoor::logger().debug("Init script Engine");
        this->errorStop = err;
        this->interval = inter;
        this->counter = 0;

        auto res = this->engine.safe_script_file(fileName, [](lua_State *, sol::protected_function_result pfr) {
            return pfr;
        });

        if (!res.valid()) {
            sol::error loadErr = res;
            trapdoor::logger().error("error load script with error {}", loadErr.what());
            this->stop();
            return false;
        }

        //build in variable
        this->bot.player = p;
        this->bs.region = &p->getRegion();


        this->engine.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math);
        //type and function binding


        this->engine.new_usertype<BotProxy>("BotProxy",
                /*information getter*/
                                            "getPos", &BotProxy::getPos,
                                            "getStandOn", &BotProxy::getStandOn,
                                            "getViewVector", &BotProxy::getViewVector,
                                            "getBlockPosFromView", &BotProxy::getBlockPosFromView,
                                            "getHealth", &BotProxy::getHealth,
                                            "getDirection", &BotProxy::getDirection,
                                            "getItemStackInfoInSlot", &BotProxy::getItemStackInfoInSlot,
                                            "getHungry", &BotProxy::getHungry,

                /*Action*/
                                            "say", &BotProxy::say,
                                            "jump", &BotProxy::jump,
                                            "lookAt", &BotProxy::lookAt,
                                            "selectItem", &BotProxy::selectItem,
                                            "destroyPosition", &BotProxy::destroyPosition,
                                            "useItemOnPosition", &BotProxy::useItemOnPosition,
                                            "interactPosition", &BotProxy::interactPosition,
                                            "useItem", &BotProxy::useItem,
                                            "dropItemInSlot", &BotProxy::dropItemInSlot,
                                            "moveTo", &BotProxy::moveTo,
                                            "runCommand", &BotProxy::runCommand,
                                            "attack", &BotProxy::attack
        );

        this->engine.new_usertype<LevelProxy>("LevelProxy",
                                              "getTick", &LevelProxy::getTick,
                                              "getTime", &LevelProxy::getTime,
                                              "getMeanMspt", &LevelProxy::getMSPT
        );

        this->engine.new_usertype<BlockPos>("BlockPos", sol::constructors<BlockPos(), BlockPos(int, int, int)>(),
                                            "x", &BlockPos::x,
                                            "y", &BlockPos::y, "z", &BlockPos::z);

        this->engine.new_usertype<Vec3>("Vec3", sol::constructors<Vec3(), Vec3(float, float, float)>(), "x", &Vec3::x,
                                        "y", &Vec3::y, "z", &Vec3::z);
        this->engine.new_usertype<BlockInfo>("BlockInfo", "id", &BlockInfo::id, "variant", &BlockInfo::variant, "name",
                                             &BlockInfo::name);
        this->engine.new_usertype<ItemStackInfo>("ItemStackInfo", "id", &ItemStackInfo::id, "aux", &ItemStackInfo::aux,
                                                 "name",
                                                 &ItemStackInfo::name,
                                                 "damage", &ItemStackInfo::damage,
                                                 "count", &ItemStackInfo::count);

        this->engine.new_usertype<BlockSourceProxy>("BlockSourceProxy", "getBlockInfo",
                                                    &BlockSourceProxy::getBlockInfo);

        this->engine.set("Bot", std::ref(this->bot));

        this->engine.set("Level", std::ref(this->level));

        this->engine.set("Region", std::ref(this->bs));
        trapdoor::logger().debug("Script init finished");

        if (!this->runFunction("Init()")) {
            if (this->errorStop)this->stop();
        }

        this->running = true;
        return true;
    }

    void ScriptDriver::tick() {
        if (!running)return;
        counter++;
        if (counter % interval != 0)return;

        if (!this->runFunction("Tick()")) {
            if (this->errorStop)this->stop();
        }

    }


    void ScriptDriver::stop() {
        //删除所有东西
        this->engine = sol::state(nullptr);
        this->running = false;
    }

    bool ScriptDriver::runFunction(const string &name) {
        auto res = this->engine.script(name, [](lua_State *, sol::protected_function_result pfr) {
            return pfr;
        });
        if (!res.valid()) {
            sol::error runErr = res;
            trapdoor::logger().error("Error in execute function {}:\n{}\n\nIn Bot {}", name, runErr.what(),
                                     this->bot.player->getRealName());
            return false;
        }
        return true;
    }

    //API -- Level
    int LevelProxy::getTime() { return Global<Level>->getTime(); }

    uint64_t LevelProxy::getTick() { return Global<Level>->getCurrentServerTick().t; }

    double LevelProxy::getMSPT() { return trapdoor::getMeanMSPT(); }

    //API -- BlockSource
    BlockInfo BlockSourceProxy::getBlockInfo(const BlockPos &pos) const {
        auto &b = this->region->getBlock(pos);
        return {
                b.getId(),
                b.getVariant(),
                b.getTypeName(),
        };
    }

    //API -- Bot
    /*getter functions*/

    int BotProxy::getDirection() const {
        return this->player->getDirection();
    }


    float BotProxy::getHungry() const {
        return this->player->getAttribute(SimulatedPlayer::HUNGER).getCurrentValue();
    }


    ItemStackInfo BotProxy::getItemStackInfoInSlot(int slot) const {
        auto &inv = this->player->getInventory();
        if (slot >= inv.getSize() || slot < 0)return {};
        auto *item = inv.getSlot(slot);
        if (!item)return {};
        return {item->getId(), trapdoor::rmmc(item->getTypeName()), item->getAux(), item->getDamageValue(),
                item->getCount()};
    }

    BlockPos BotProxy::getStandOn() const {
        return this->player->getBlockStandingOn().getPosition();
    }

    Vec3 BotProxy::getPos() const {
        return this->player->getPosition();
    }

    Vec3 BotProxy::getViewVector() const {
        return this->player->getViewVector(1.0f);
    }

    BlockPos BotProxy::getBlockPosFromView() const {
        return this->player->getBlockFromViewVector().getPosition();
    }

    int BotProxy::getHealth() const {
        return this->player->getHealth();
    }

    /********************************Actions********************************/

    void BotProxy::say(const string &msg) const {
        trapdoor::broadcastMessage(fmt::format(" <{}> {}", this->player->getRealName(), msg));
    }


    bool BotProxy::jump() const {
        return player->simulateJump();
    }

    void BotProxy::moveTo(const Vec3 &pos) const {
        player->simulateMoveToLocation(pos, 1.0f);
    }

    bool BotProxy::selectItem(const string &name) const {
        return bot::switchItemToHandByName(this->player, name);
    }


    bool BotProxy::destroyPosition(const BlockPos &pos) const {
        return player->simulateDestroyBlock(pos, DF);
    }


    bool BotProxy::attack() const {
        return player->simulateAttack();
    }


    bool BotProxy::runCommand(const string &cmd) const {
        player->runcmd(cmd);
        return true;
    }


    void BotProxy::lookAt(const Vec3 &v) const {
        this->player->simulateLookAt(v);
    }

    int BotProxy::useItemOnPosition(const string &name, const BlockPos &pos, int face) const {
        if (bot::switchItemToHandByName(this->player, name)) {
            return this->player->simulateUseItemOnBlock(*bot::getSelectItem(this->player), pos,
                                                        static_cast<ScriptModuleMinecraft::ScriptFacing>(face),
                                                        {0.5, 0.5, 0.5}) ? 0 : 1;
        } else {
            return -1;
        }
    }

    int BotProxy::useItem(const string &name) const {
        if (bot::switchItemToHandByName(this->player, name)) {
            return this->player->simulateUseItem(*bot::getSelectItem(this->player)) ? 0 : 1;
        } else {
            return -1;
        }
    }

    bool BotProxy::interactPosition(const BlockPos &pos, int face) const {
        return this->player->simulateInteract(pos, static_cast<ScriptModuleMinecraft::ScriptFacing>(face));
    }

    void BotProxy::dropItemInSlot(int slot) const {
        auto &inv = this->player->getInventory();
        if (slot >= inv.getSize() || slot < 0) return;
        auto *item = inv.getSlot(slot);
        this->player->drop(*item, false);
        this->player->getInventory().removeItem(slot, 64);
        this->player->sendInventory(true);
    }

}
