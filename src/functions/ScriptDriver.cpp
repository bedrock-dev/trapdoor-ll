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

namespace trapdoor {

    bool ScriptDriver::init(const string &fileName, SimulatedPlayer *p, int inter, bool err) {
        this->errorStop = err;
        this->interval = inter;
        this->counter = 0;
        auto res = this->engine.script_file(fileName);
        if (!res.valid()) {
            return false;
        }

        //build in variable
        this->bot.player = p;
        this->bs.region = &p->getRegion();


        this->engine.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math);
        //type and function binding

#define  BIND(func) #func,&BotProxy::##func

        this->engine.new_usertype<BotProxy>("BotProxy",
                                            "getPosition", &BotProxy::getPosition,
                                            "getStandBlockPos", &BotProxy::getStandBlockPos,
                                            "getViewVector", &BotProxy::getViewVector,
                                            "getBlockPosFromView", &BotProxy::getBlockPosFromView,
                                            "say", &BotProxy::say,
                                            "destroyPosition", &BotProxy::destroyPosition,
                                            "lookAtVec3", &BotProxy::lookAtVec3,
                //the api below need test !!
                                            "moveto", &BotProxy::moveto,
                                            "jump", &BotProxy::jump,
                                            "runCommand", &BotProxy::runCommand
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
        this->engine.new_usertype<BlockSourceProxy>("BlockSourceProxy", "getBlockInfo",
                                                    &BlockSourceProxy::getBlockInfo);

        this->engine.set("Bot", std::ref(this->bot));

        this->engine.set("Level", std::ref(this->level));

        this->engine.set("Region", std::ref(this->bs));

        this->running = true;
        return true;
    }

    void ScriptDriver::tick() {
        if (!running)return;
        counter++;
        if (counter % interval != 0)return;
        try {
            this->engine.safe_script("Tick()");
        } catch (std::exception &e) {
            this->stop();
            trapdoor::broadcastMessage(
                    fmt::format("Script running error for sim player {}, visit the console for more info",
                                this->bot.player->getRealName()));
            trapdoor::logger().error("LUA run error:\n {} \nwith player {}", e.what(),
                                     this->bot.player->getRealName());
        }
    }


    void ScriptDriver::stop() {
        //删除所有东西
        this->engine = sol::state(nullptr);
        this->running = false;
    }

    //API -- Level
    int LevelProxy::getTime() { return Global<Level>->getTime(); }

    uint64_t LevelProxy::getTick() { return Global<Level>->getCurrentServerTick().t; }

    double LevelProxy::getMSPT() { return trapdoor::getMeanMSPT(); }


    //API -- Bot
    void BotProxy::say(const string &msg) const {
        trapdoor::broadcastMessage(fmt::format(" <{}> {}", this->player->getRealName(), msg));
    }


    void BotProxy::jump() const {
        player->simulateJump();
    }

    void BotProxy::moveto(float px, float py, float pz) const {
        player->simulateMoveToLocation({px, py, pz}, 1.0f);
    }

    void BotProxy::update() {
        auto pos = player->getPos();
    }

    void BotProxy::destroyPosition(const BlockPos &pos) const {
        player->simulateDestroyBlock(pos, DF);
    }


    void BotProxy::interact() const {
        player->simulateInteract();
    }

    void BotProxy::attack() const {
        player->simulateAttack();
    }

    bool BotProxy::runCommand(const string &cmd) const {
        player->runcmd(cmd);
        return true;
    }

    BlockPos BotProxy::getStandBlockPos() const {
        return this->player->getBlockStandingOn().getPosition();
    }

    Vec3 BotProxy::getPosition() const {
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


    //todo
    int BotProxy::getHungry() const {

        return 0;
    }

    void BotProxy::lookAtVec3(const Vec3 &v) const {
        this->player->simulateLookAt(v);
    }


    BlockInfo BlockSourceProxy::getBlockInfo(const BlockPos &pos) const {
        auto &b = this->region->getBlock(pos);
        return {
                b.getId(),
                b.getVariant(),
                b.getTypeName(),
        };
    }
}
