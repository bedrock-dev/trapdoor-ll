//
// Created by xhy on 2023/3/10.
//

#ifndef TRAPDOOR_SCRIPTDRIVER_H
#define TRAPDOOR_SCRIPTDRIVER_H

#include <mc/BlockInstance.hpp>
#include <mc/SimulatedPlayer.hpp>

#include "MCTick.h"
#include "Msg.h"
#include "sol.hpp"

namespace trapdoor {
    constexpr auto DF = static_cast<ScriptModuleMinecraft::ScriptFacing>(1);

    // BOT API

    struct BlockInfo {
        int id{0};
        int variant{0};
        const std::string name;
    };

    struct ItemStackInfo {
        int id{0};
        const std::string name;
        int aux{0};
        short damage{0};
        int count{0};
    };

    struct BotProxy {

    public:

        /***************************Info get functions***************************/
        //获取站立的方块坐标
        [[nodiscard]] BlockPos getStandOn() const;

        // 获取自身坐标
        [[nodiscard]] Vec3 getPos() const;

        // 获取视角向量
        [[nodiscard]] Vec3 getViewVector() const;

        // 获取看向的方块坐标
        [[nodiscard]] BlockPos getBlockPosFromView() const;

        //获取饥饿值
        [[nodiscard]] float getHungry() const;

        // 获取血量
        [[nodiscard]] int getHealth() const;

        //获取方向
        [[nodiscard]] int getDirection() const;

        //获取背包某个位置的物品信息
        [[nodiscard]] ItemStackInfo getItemStackInfoInSlot(int slot) const;

        /******************Action functions********************/

    public:
        // 向服务区广播消息
        void say(const std::string &msg) const;

        // 跳跃 返回是否跳跃成功
        [[nodiscard]] bool jump() const;

        //看着某个地方
        void lookAt(const Vec3 &v) const;

        //从背包搜索物品并放到主手 返回操作是否成功（即有这个物品就返回true）
        [[nodiscard]] bool selectItem(const std::string &name) const;

        // 破坏某个位置的方块,返回是否操作成功
        [[nodiscard]] bool destroyPosition(const BlockPos &pos) const;

        // 使用某物品右键某个方块,成功返回0，没有物品返回-1，有物品但是操作失败返回1
        [[nodiscard]] int useItemOnPosition(const std::string &name, const BlockPos &pos,
                                            int face) const;

        //和某个位置交互（空手右键）返回操作是否成功
        [[nodiscard]] bool interactPosition(const BlockPos &pos, int face) const;


        // 右键某物品（吃东西等）,返回操作是否成功
        [[nodiscard]] bool useItem(const std::string &name) const;

        //攻击 返回操作是否成功
        [[nodiscard]] bool attack() const;

        //丢弃某个位置的物品
        void dropItemInSlot(int slot) const;

        //移动到某个位置
        void moveTo(const Vec3 &pos) const;

        //运行命令 返回是否执行成功
        [[nodiscard]] bool runCommand(const std::string &cmd) const;

    public:

        SimulatedPlayer *player{nullptr};
    };

    struct BlockSourceProxy {
        inline void setRegion(BlockSource *bs) { this->region = bs; }

        [[nodiscard]] BlockInfo getBlockInfo(const BlockPos &pos) const;

        BlockSource *region{nullptr};
    };

    struct LevelProxy {
        int getTime();

        uint64_t getTick();

        double getMSPT();
    };

    // driver
    class ScriptDriver {
    public:
        bool init(const string &fileName, SimulatedPlayer *p, int interval, bool errorStop);

        void tick();

        [[nodiscard]] inline bool isRunning() const { return this->running; }

        void stop();

        ScriptDriver() = default;

    private:
        sol::state engine;  // engine
        BotProxy bot;       // for data binding
        BlockSourceProxy bs{};
        LevelProxy level;
        bool running{false};
        bool errorStop{true};
        int interval{1};
        uint64_t counter{0};
    };
}  // namespace trapdoor

#endif  // TRAPDOOR_SCRIPTDRIVER_H
