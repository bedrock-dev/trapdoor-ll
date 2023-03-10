//
// Created by xhy on 2023/3/10.
//

#include "ScriptDriver.h"

#include "TrapdoorMod.h"

namespace trapdoor {

    bool ScriptDriver::init(const string &fileName, SimulatedPlayer *p, int inter, bool err) {
        this->errorStop = err;
        this->interval = inter;
        this->counter = 0;
        this->engine.stack_clear();
        auto res = this->engine.script_file(fileName);
        if (!res.valid()) {
            return false;
        }

        //build in variable
        this->bot.player = p;
        this->engine.new_usertype<BotProxy>("Bot",
                                            "x", &BotProxy::x,
                                            "y", &BotProxy::y,
                                            "z", &BotProxy::z,
                                            "say", &BotProxy::say,
                                            "moveto", &BotProxy::moveto,
                                            "jump", &BotProxy::jump

        );

        this->engine.set("Bot", std::ref(this->bot));
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
}
