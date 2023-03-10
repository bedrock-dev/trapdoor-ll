//
// Created by xhy on 2023/3/10.
//

#include "ScriptDriver.h"

#include "TrapdoorMod.h"

namespace trapdoor {

    bool ScriptDriver::init(const string &fileName, SimulatedPlayer *p) {
        this->engine.stack_clear();
        auto res = this->engine.script_file(fileName);
        if (!res.valid()) {
            return false;
        }

        //build in variable
        this->bot.player = p;
        this->engine.new_usertype<Bot>("Bot",
                                       "x", &Bot::x,
                                       "y", &Bot::y,
                                       "z", &Bot::z,
                                       "say", &Bot::say,
                                       "moveto", &Bot::moveto,
                                       "jump", &Bot::jump

        );

        this->engine.set("bot", std::ref(this->bot));
        this->running = true;
        return true;
    }


    void ScriptDriver::tick() {
        if (!running)return;
        this->engine.script("Tick()");
    }

    void ScriptDriver::stop() {
        //删除所有东西
        this->engine.stack_clear();
    }
}
