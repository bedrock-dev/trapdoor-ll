//
// Created by xhy on 2022/7/9.
//

#ifndef TRAPDOOR_SYSINFOHELPER_H
#define TRAPDOOR_SYSINFOHELPER_H
#include <cstdint>

#include "CommandHelper.h"
// from
// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

namespace tr {

    struct MemInfo {
        size_t totalPys = 0;
        size_t curTotalUsed = 0;
        size_t curProcessUsed = 0;
    };

    struct CPUInfo {
        double totalUsage;
        double curUsage;
        size_t numOfProc;
    };

    MemInfo getMemInfo();
    CPUInfo getCPUInfo();

    void initCPU();

    ActionResult printSysInfo();
}  // namespace tr

#endif  // TRAPDOOR_SYSINFOHELPER_H
