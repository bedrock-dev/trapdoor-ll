#include "Shortcuts.h"

#include <FMT/format.h>

#include <MC/Block.hpp>
#include <MC/ItemStack.hpp>

#include "TrapdoorMod.h"
namespace trapdoor {
    namespace {

        void parsePattern(const std::string& str, std::string& name, int& aux) {
            auto p = str.find(':');
            if (p == std::string::npos) {
                name = str;
                aux = -1;
                return;
            }
            name = str.substr(0, p);
            aux = std::stoi(str.substr(p + 1));
        }

        bool patternMatch(const std::string& str, int aux, const std::string& str2, int aux2) {
            if (str.empty()) {               // 空的名字肯定匹配
                if (aux == -1) return true;  // aux == -1 特殊值也一定匹配
                return aux == aux2;          // 检查特殊值
            } else {
                if (str != str2) return false;  // 名称不匹配
                if (aux == -1) return true;     // aux == -1 特殊值一定匹配
                return aux == aux2;             // 检查aux
            }
        }
    }  // namespace

    void Shortcut::setItem(const string& str) { parsePattern(str, itemName, itemAux); }
    void Shortcut::setBlock(const string& str) { parsePattern(str, blockName, blockAux); }
    bool Shortcut::operator==(const Shortcut& rhs) const {
        return type == rhs.type && itemName == rhs.itemName && itemAux == rhs.itemAux &&
               blockName == rhs.blockName && blockAux == rhs.blockAux;
    }
    bool Shortcut::operator!=(const Shortcut& rhs) const { return !(rhs == *this); }
    bool Shortcut::operator<(const Shortcut& rhs) const {
        if (type < rhs.type) return true;
        if (rhs.type < type) return false;
        if (itemName < rhs.itemName) return true;
        if (rhs.itemName < itemName) return false;
        if (itemAux < rhs.itemAux) return true;
        if (rhs.itemAux < itemAux) return false;
        if (blockName < rhs.blockName) return true;
        if (rhs.blockName < blockName) return false;
        return blockAux < rhs.blockAux;
    }
    std::string Shortcut::getDescription() const {
        std::string str;
        switch (this->type) {
            case USE:
                str += "use [" + itemName + ":" + std::to_string(itemAux) + "] ";
                break;
            case USE_ON:
                str += "use [" + itemName + ":" + std::to_string(itemAux) + "] on [" + blockName +
                       ":" + std::to_string(blockAux) + "]  ";

                break;
            case CMD:
                str += "command";
                break;
            case DESTROY:
                str += "use [" + itemName + ":" + std::to_string(itemAux) + "] destroy [" +
                       blockName + ":" + std::to_string(blockAux) + "]  ";
        }

        str += "prevent: " + std::to_string(prevent) + "  ";
        str += "actions: ";
        for (auto& ac : actions) {
            str += ac + ",";
        }
        return str;
    }
    bool Shortcut::match(const Shortcut& shortcut) const {
        if (type != shortcut.type) return false;
        if (shortcut.type == USE) {
            return patternMatch(itemName, itemAux, shortcut.itemName, shortcut.itemAux);
        } else if (shortcut.type == USE_ON || shortcut.type == DESTROY) {
            return patternMatch(itemName, itemAux, shortcut.itemName, shortcut.itemAux) &&
                   patternMatch(blockName, blockAux, shortcut.blockName, shortcut.blockAux);
        }
        return false;
    }
    void Shortcut::runUse(Player* player, ItemStack* item) {
        auto pos = player->getPos().toBlockPos();
        for (auto& act : actions) {
            auto cmd = fmt::format(act, fmt::arg("px", pos.x), fmt::arg("py", pos.y),
                                   fmt::arg("pz", pos.z), fmt::arg("iname", item->getName()),
                                   fmt::arg("iaux", item->getAux()));
            trapdoor::logger().debug("cmd is {}", cmd);
            player->runcmd(cmd);
        }
    }
    void Shortcut::runUseOn(Player* player, const ItemStack* item, Block* block,
                            const BlockPos& p) {
        auto pos = player->getPos().toBlockPos();
        for (auto& act : actions) {
            auto cmd =
                fmt::format(act, fmt::arg("px", pos.x), fmt::arg("py", pos.y),
                            fmt::arg("pz", pos.z), fmt::arg("bname", block->getName().getString()),
                            fmt::arg("baux", block->getVariant()), fmt::arg("bx", p.x),
                            fmt::arg("by", p.y), fmt::arg("bz", p.z),
                            fmt::arg("iname", item->getName()), fmt::arg("iaux", item->getAux()));
            trapdoor::logger().debug("cmd is {}", cmd);
            player->runcmd(cmd);
        }
    }
    void Shortcut::runUseDestroy(Player* player, const ItemStack* item, Block* block,
                                 const BlockPos& p) {
        return runUseOn(player, item, block, p);
    }
}  // namespace trapdoor
