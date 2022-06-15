#ifndef TRAPDOOR_SHORTCUTS_H
#define TRAPDOOR_SHORTCUTS_H
#include <MC/Player.hpp>
#include <string>
#include <vector>
namespace tr {
    enum ShortcutType { USE = 0, USE_ON = 1, CMD };
    struct Shortcut {
        ShortcutType type;     // 类型
        std::string itemName;  // 使用的物品
        int itemAux = -1;
        std::string blockName;  // 使用的方块
        int blockAux = -1;
        bool prevent = false;

        std::vector<std::string> actions;  // 命令
       public:
        void setItem(const std::string& str);
        void setBlock(const std::string& str);
        bool operator==(const Shortcut& rhs) const;
        bool operator!=(const Shortcut& rhs) const;
        bool operator<(const Shortcut& rhs) const;
        std::string getDescription() const;
        void runUse(Player* player, ItemStack* item);
        void runUseOn(Player* player, ItemStack* item, Block* block, const BlockPos& p);
        bool match(const Shortcut& shortcut) const;
    };

}  // namespace tr
#endif