#include "TrapdoorMod.h"
#include "pch.h"

inline void CheckProtocolVersion() {}

void PluginInit() {
    CheckProtocolVersion();
    const char *s =
#include "banner.txt"
        ;
    printf("%s", s);
    tr::mod().init();
}
