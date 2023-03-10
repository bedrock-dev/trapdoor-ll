//
// Created by xhy on 2023/3/10.
//

#ifndef TRAPDOOR_SCRIPTDRIVER_H
#define TRAPDOOR_SCRIPTDRIVER_H

#include "sol.hpp"
#include "MCTick.h"
#include <mc/SimulatedPlayer.hpp>
#include "Msg.h"

namespace trapdoor {
    constexpr auto DF = static_cast<ScriptModuleMinecraft::ScriptFacing>(1);

    //BOT API
    struct BotProxy {
        void say(const std::string &msg) const;

        inline void jump() const;

        inline void moveto(float px, float py, float pz) const;

        inline void update();


        inline void destroyPosition(int px, int py, int pz) const;

        inline void interact() const;

        inline void attack() const;

        [[nodiscard]] bool runCommand(const std::string &cmd) const;

        SimulatedPlayer *player{nullptr};
        float x;
        float y;
        float z;
    };


    struct BlockSourceProxy {
        inline void setRegion(BlockSource *bs) { this->region = bs; }

        std::string getBlockName(int x, int y, int z);

        int getBlockID(int x, int y, int z);

        BlockSource *region;

    };

    struct LevelProxy {
        int getTime();

        uint64_t getTick();

        double getMSPT();


    };

    //driver
    class ScriptDriver {
    public:

        bool init(const string &fileName, SimulatedPlayer *p, int interval, bool errorStop);

        void tick();

        [[nodiscard]] inline bool isRunning() const { return this->running; }

        void stop();

        ScriptDriver() = default;

    private:
        sol::state engine; //engine
        BotProxy bot; //for data binding
        BlockSourceProxy bs;
        LevelProxy level;
        bool running{false};
        bool errorStop{true};
        int interval{1};
        uint64_t counter{0};
    };
}

#endif //TRAPDOOR_SCRIPTDRIVER_H
