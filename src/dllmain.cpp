// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <LLAPI.h>

#include <cstdint>

#include "Version.h"
#include "pch.h"

extern "C" int64_t __pfnDliNotifyHook2 = 0;
extern "C" int64_t __pfnDliFailureHook2 = 0;

#pragma comment(lib, "../SDK-cpp/lib/bedrock_server_api.lib")
#pragma comment(lib, "../SDK-cpp/lib/bedrock_server_var.lib")
#pragma comment(lib, "../SDK-cpp/lib/SymDBHelper.lib")
#pragma comment(lib, "../SDK-cpp/lib/LiteLoader.lib")
#include "config.h"
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
#ifdef DEV
    auto v = ll::Version::Status::Dev;
#else
    auto v = ll::Version::Status::Release;
#endif

    auto version =
        ll::Version(PLUGIN_VERSION_MAJOR, PLUGIN_VERSION_MINOR, PLUGIN_VERSION_REVISION, v);
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            ll::registerPlugin(PLUGIN_NAME, PLUGIN_INTRODUCTION, version,
                               "https:github.com/bedrock-dev/trapdoor-ll", "AGPL",
                               "https:github.com/bedrock-dev/trapdoor-ll");
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

void PluginInit();

extern "C" {
// Do something after all the plugins loaded
_declspec(dllexport) void onPostInit() {
    std::ios::sync_with_stdio(false);
    PluginInit();
}
}
