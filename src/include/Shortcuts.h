#ifndef TRAPDOOR_SHORTCUTS_H
#define TRAPDOOR_SHORTCUTS_H
#include <MC/Player.hpp>
#include <string>
namespace tr {
    enum ShortcutType { USE = 0, USEON = 1, CMD = 2 };
    struct Shortcut {
        ShortcutType type;    // 类型
        std::string item;     // 使用的物品
        std::string block;    // 使用的方块
        std::string command;  // 触发的快捷指令
        std::string action;   // 行为
        bool operator<(const Shortcut& rhs) const;
        bool operator==(const Shortcut& rhs) const;

        std::string getDescription() const {
            std::string desc;
            switch (type) {
                case USE:
                    desc = "use " + item;
                    break;
                case USEON:
                    desc = "use " + item + " on " + block;
                    break;
                case CMD:
                    desc = "command /" + command;
                    break;
            }
            return desc + ":    action: /" + action;
        }
    };

    void runShortcut(const Shortcut& shortcut, Player* player);
}  // namespace tr
#endif