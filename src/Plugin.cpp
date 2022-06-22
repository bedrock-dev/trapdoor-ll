#include <filesystem>

#include "TrapdoorMod.h"
#include "pch.h"

inline void CheckProtocolVersion() {}

void prepare() {
    namespace fs = std::filesystem;
    if (!fs::exists("./plugins/trapdoor")) {
        fs::create_directory("./plugins/trapdoor");
    }
}

void PluginInit() {
    CheckProtocolVersion();
    const char *s =
#include "banner.txt"
        ;
    printf("%s", s);
    prepare();
    tr::mod().init();
}
