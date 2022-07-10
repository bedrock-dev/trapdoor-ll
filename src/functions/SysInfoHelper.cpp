// Created by xhy on 2022/7/9.
// clang-format off
#include <Windows.h>
#include <Psapi.h>
#include <Pdh.h>
// clang-format on
#include "SysInfoHelper.h"

#include "Msg.h"
namespace tr {

    MemInfo getMemInfo() {
        MEMORYSTATUSEX info;
        info.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&info);

        MemInfo res;
        res.totalPys = info.ullTotalPhys;
        res.curTotalUsed = info.ullTotalPhys - info.ullAvailPhys;

        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        res.curProcessUsed = pmc.WorkingSetSize;
        return res;
    }

    namespace {
        // Total CPU Usage
        PDH_HQUERY cpuQuery;
        PDH_HCOUNTER cpuTotal;

        // Current
        ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
        size_t numProcessors;
        HANDLE self;
        bool cpu_init = false;

        double getTotalCPUUsage() {
            PDH_FMT_COUNTERVALUE counterVal;
            PdhCollectQueryData(cpuQuery);
            PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
            return counterVal.doubleValue;
        }

        double getCurrentCPUsage() {
            FILETIME ftime, fsys, fuser;
            ULARGE_INTEGER now, sys, user;
            double percent;

            GetSystemTimeAsFileTime(&ftime);
            memcpy(&now, &ftime, sizeof(FILETIME));

            GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
            memcpy(&sys, &fsys, sizeof(FILETIME));
            memcpy(&user, &fuser, sizeof(FILETIME));
            percent = static_cast<double>((sys.QuadPart - lastSysCPU.QuadPart) +
                                          (user.QuadPart - lastUserCPU.QuadPart));
            percent /= static_cast<double>(now.QuadPart - lastCPU.QuadPart);
            percent /= static_cast<double>(numProcessors);
            lastCPU = now;
            lastUserCPU = user;
            lastSysCPU = sys;
            return percent * 100;
        }
    }  // namespace

    void init() {
        PdhOpenQuery(nullptr, NULL, &cpuQuery);
        // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with
        // PdhGetFormattedCounterArray()
        PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
        PdhCollectQueryData(cpuQuery);

        //
        SYSTEM_INFO sysInfo;
        FILETIME ftime, fsys, fuser;
        GetSystemInfo(&sysInfo);
        numProcessors = sysInfo.dwNumberOfProcessors;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&lastCPU, &ftime, sizeof(FILETIME));

        self = GetCurrentProcess();
        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
    }
    CPUInfo getCPUInfo() {
        if (!cpu_init) {
            init();
            cpu_init = true;
        }
        return {getTotalCPUUsage(), getCurrentCPUsage(), numProcessors};
    }
    ActionResult printSysInfo() {
        tr::TextBuilder builder;
        auto mem = getMemInfo();
        auto cpu = getCPUInfo();

        builder.text("MEM: ").textF("Total: %zuMB  CurUsage: %zuMB TotalUsage: %zuMB\n",
                                    mem.totalPys >> 20, mem.curProcessUsed >> 20,
                                    mem.curTotalUsed >> 20);

        builder
            .text("CPU: ")
            // total
            .text("TotalUsage: ")
            .textF("%.2lf", cpu.totalUsage)
            .text("%%   ")
            // cur
            .text("CurUsage: ")
            .textF("%.2lf", cpu.curUsage)
            .text("%%   ")
            // processor
            .text("Processor: ")
            .textF("%zu", cpu.numOfProc)
            .text("%");

        return {builder.get(), true};
    }
    void initCPU() { auto _ = getCPUInfo(); }
}  // namespace tr
