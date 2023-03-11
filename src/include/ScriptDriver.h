//
// Created by xhy on 2023/3/10.
//

#ifndef TRAPDOOR_SCRIPTDRIVER_H
#define TRAPDOOR_SCRIPTDRIVER_H

#include "sol.hpp"
#include "MCTick.h"
#include <mc/SimulatedPlayer.hpp>
#include <mc/BlockInstance.hpp>

#include "Msg.h"

namespace trapdoor {
    constexpr auto DF = static_cast<ScriptModuleMinecraft::ScriptFacing>(1);

    //BOT API



    struct BotProxy {

        //getStandardPosition
        [[nodiscard]] BlockPos getStandBlockPos() const;


        //获取坐标
        [[nodiscard]] Vec3 getPosition() const;

        //获取视角
        [[nodiscard]] Vec3 getViewVector() const;

        //获取看向的方块
        [[nodiscard]] BlockPos getBlockPosFromView() const;


        //获取自身的饥饿度
        int getHungry() const;

        //获取血量
        int getHealth() const;

        //向服务器广播消息
        void say(const std::string &msg) const;

        //跳跃
        void jump() const;

        //破坏方块
        void destroyPosition(const BlockPos &pos) const;

        void lookAtVec3(const Vec3 &v) const;

        // void useOnPosition();
        //

        void moveto(float px, float py, float pz) const;


        void update();


        void interact() const;

        void attack() const;

        [[nodiscard]] bool runCommand(const std::string &cmd) const;

        SimulatedPlayer *player{nullptr};

    };


    struct BlockInfo {
        int id;
        int variant;
        const std::string name;
    };

    struct BlockSourceProxy {
        inline void setRegion(BlockSource *bs) { this->region = bs; }

        [[nodiscard]] BlockInfo getBlockInfo(const BlockPos &pos) const;

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
        BlockSourceProxy bs{};
        LevelProxy level;
        bool running{false};
        bool errorStop{true};
        int interval{1};
        uint64_t counter{0};
    };
}

#endif //TRAPDOOR_SCRIPTDRIVER_H
