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

        //function binding
        this->engine.new_usertype<BotProxy>("Bot",
                                            "x", &BotProxy::x,
                                            "y", &BotProxy::y,
                                            "z", &BotProxy::z,
                                            "say", &BotProxy::say,
                                            "moveto", &BotProxy::moveto,
                                            "jump", &BotProxy::jump,
                                            "runcmd", &BotProxy::runCommand,
                                            "destroyPosition", &BotProxy::destroyPosition
        );

        this->engine.new_usertype<LevelProxy>("Level",
                                              "getTick", &LevelProxy::getTick,
                                              "getTime", &LevelProxy::getTime,
                                              "getMeanMspt", &LevelProxy::getMSPT

        );


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
            this->engine.script("Tick()");
        } catch (std::exception &e) {
            this->stop();
            trapdoor::broadcastMessage(
                    fmt::format("Script running error for sim player {}, visit the console for more info",
                                this->bot.player->getRealName()));
            trapdoor::logger().error("LUA run error:{} with player {}", e.what(), this->bot.player->getRealName());
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
        x = pos.x;
        y = pos.y;
        z = pos.z;
    }

    void BotProxy::destroyPosition(int px, int py, int pz) const {
        player->simulateDestroyBlock({px, py, pz}, DF);
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

    std::string BlockSourceProxy::getBlockName(int x, int y, int z) {
        return this->region->getBlock(x, y, z).getTypeName();
    }

    int BlockSourceProxy::getBlockID(int x, int y, int z) {
        return this->region->getBlock(x, y, z).getId();
    }


}
