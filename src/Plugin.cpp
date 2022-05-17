#include "pch.h"
#include "TrapdoorMod.h"

inline void CheckProtocolVersion() {}

void PluginInit() {
    CheckProtocolVersion();
    tr::InitMod();
}
