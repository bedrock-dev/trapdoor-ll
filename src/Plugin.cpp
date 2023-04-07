#include <filesystem>

#include "TrapdoorMod.h"
#include "pch.h"

inline void CheckProtocolVersion() {}

// void prepare() {
//     tryCreateFolder("./plugins/trapdoor");
//     tryCreateFolder("./plugins/trapdoor/sim");
//     tryCreateFolder("./plugins/trapdoor/player");
//     tryCreateFolder("./plugins/trapdoor/logs");
//     tryCreateFolder("./plugins/trapdoor/scripts");
// }

void PluginInit() {
    CheckProtocolVersion();
    trapdoor::mod().init();
}
