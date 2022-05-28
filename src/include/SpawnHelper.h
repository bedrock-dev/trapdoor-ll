#ifndef TRAPDOOR_SPAWN_HELPER_H
#define TRAPDOOR_SPAWN_HELPER_H
#include <MC/Player.hpp>

#include "CommandHelper.h"

namespace tr {
    ActionResult PrintCap();

    ActionResult CountActors(Player *p, const std::string &type);
}  // namespace tr
#endif