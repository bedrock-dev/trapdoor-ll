//
// Created by xhy on 2023/3/5.
//
#include "RemoteCallAPI.h"
#include "TrapdoorAPI.h"
#include "MCTick.h"

#define  TR Trapdoor

#define FN(name) "Trapdoor",#name
namespace trapdoor {
    void exportAPI() {


        RemoteCall::exportAs(FN(setFunction), [](const std::string &name, bool) {
            //TODO 开启或关闭某项目功能
        });

        RemoteCall::exportAs(FN(getFunction), [](const std::string &name, bool) {
            //TODO 查询某项功能是否开启
        });

        RemoteCall::exportAs(FN(getRedstoneComponent), [] {
            //TODO 获取红石元件信息
        });

        RemoteCall::exportAs(FN(getHopperCounterData), [](int channel) -> void {
            //TODO 获取漏斗频道的数据
        });

        RemoteCall::exportAs(FN(clearHopperCounterData), [](int channel) -> void {
            //TODO 清空漏斗频道的数据
        });

        RemoteCall::exportAs(FN(getHSAList), []() {
            //TODO: 获取收集的HSA信息
        });

        RemoteCall::exportAs(FN(getMeanMspt), []() -> double {
            //获取平均MSPT
            return trapdoor::getMeanMSPT();
        });

        RemoteCall::exportAs(FN(getMeanTPS), []() -> double {
            //获取平均TPS
            return trapdoor::getMeanTPS();
        });
        RemoteCall::exportAs(FN(getSpawnContidion), []() {
            //TODO 获取生物生成条件
        });

        RemoteCall::exportAs(FN(forceSpawn), []() {
            //TODO 强制生物生成
        });

        RemoteCall::exportAs(FN(queryWorld), []() {
            //TODO 查询世界加速状态
        });

        RemoteCall::exportAs(FN(getSimPlayerList), [] {
            //TODO 获取假人列表
        });


        RemoteCall::exportAs(FN(getVillageLust), [] {
            //TODO 获取正在加载的村庄列表
        });

        RemoteCall::exportAs(FN(getVillageInfo), [](const std::string &uid) {
            //TODO: 获取特定UID的村庄的信息
        });

    }
}


