//
// Created by xhy on 2020/8/24.
//

#ifndef TRAPDOOR_MSGBUILDER_H
#define TRAPDOOR_MSGBUILDER_H

#include <cstdarg>
#include <map>
#include <string>
#include <vector>

#define PH_DARK_RED "§4{}§r"
#define PH_RED "§c{}§r"
#define PH_GOLD "§6{}§r"
#define PH_DARK_GREEN "§2{}§r"
#define PH_YELLOW "§e{}§r"
#define PH_GREEN "§a{}§r"
#define PH_AQUA "§b{}§r"
#define PH_BLUE "§9{}§r"
#define PH_LIGHT_PURLPE "§d{}§r"
#define PH_WHITE "§f{}§r"
#define PH_GRAY "§7{}§r"
#define PH_DOUB "§f{:.3lf}§r"

#define PH_NUM "§2{}§r"
#define PH_POS "§a{},{},{}§r"

namespace tr {
    void BroadcastMessage(const std::string& msg, int level = 0);
}  // namespace tr
#endif  // TRAPDOOR_MSGBUILDER_H
