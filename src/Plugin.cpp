#include <ctype.h>

#include "TrapdoorMod.h"
#include "pch.h"

inline void CheckProtocolVersion() {}

void PluginInit() {
    CheckProtocolVersion();
    tr::mod().init();
}
