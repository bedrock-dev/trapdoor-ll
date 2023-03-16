#include <filesystem>

#include "TrapdoorMod.h"
#include "pch.h"

inline void CheckProtocolVersion() {}


void tryCreateFolder(const std::string &folder) {
    namespace fs = std::filesystem;
    if (!fs::exists(folder)) {
        fs::create_directory(folder);
    }

}

void prepare() {

    tryCreateFolder("./plugins/trapdoor");
    tryCreateFolder("./plugins/trapdoor/sim");
    tryCreateFolder("./plugins/trapdoor/player");
    tryCreateFolder("./plugins/trapdoor/logs");
    tryCreateFolder("./plugins/trapdoor/scripts");
}

void PluginInit() {
    CheckProtocolVersion();
    //    const char *s =
    // #include "banner.txt"
    //        ;
    //    printf("%s", s);
    prepare();
    trapdoor::mod().init();
}
