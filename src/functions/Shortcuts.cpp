#include "Shortcuts.h"

#include "TrapdoorMod.h"
namespace tr {
    void runShortcut(const tr::Shortcut& shortcut, Player* player) {
        if (!player) return;
        tr::logger().debug("player {} trigger: {}", player->getRealName(),
                           shortcut.getDescription());
        auto& shs = tr::mod().getConfig().getShortcuts();
        auto iter = shs.find(shortcut);
        if (iter != shs.end()) {
            player->runcmd(iter->action);
        }
    }

    bool Shortcut::operator<(const Shortcut& rhs) const {
        if (type < rhs.type) return true;
        if (rhs.type < type) return false;
        if (item < rhs.item) return true;
        if (rhs.item < item) return false;
        if (block < rhs.block) return true;
        if (rhs.block < block) return false;
        return command < rhs.command;
    }
    bool Shortcut::operator==(const Shortcut& rhs) const {
        return type == rhs.type && item == rhs.item && block == rhs.block &&
               command == rhs.command && action == rhs.action;
    }

}  // namespace tr
