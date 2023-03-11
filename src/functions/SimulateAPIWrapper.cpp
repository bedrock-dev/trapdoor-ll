//
// Created by xhy on 2023/3/11.
//

#include "SimulateAPIWrapper.h"
#include "TrapdoorMod.h"


namespace trapdoor::bot {

    ItemStack *searchFirstItemInInv(SimulatedPlayer *sim, int &slot,
                                    const std::function<bool(const ItemStack *)> &filter) {
        if (!sim) return nullptr;
        slot = -1;
        auto &inv = sim->getInventory();
        int sz = inv.getSize();
        for (int i = 0; i < sz; i++) {
            auto *item = inv.getSlot(i);
            if (item && filter(item)) {
                slot = i;
                return item;
            }
        }
        return nullptr;
    }


    ItemStack *searchFirstItemInInvByName(SimulatedPlayer *sim, int &slot, const string &name) {
        return searchFirstItemInInv(sim, slot, [&](const ItemStack *item) -> bool {
            return rmmc(item->getTypeName()) == rmmc(name);
        });
    }

    ItemStack *searchFirstItemInInvById(SimulatedPlayer *sim, int &slot, int id) {
        return searchFirstItemInInv(sim, slot, [&](const ItemStack *item) -> bool {
            return item->getId() == id;
        });
    }

    void swapTwoSlotInInventory(Container &cont, int s1, int s2) {
        if (s1 < 0 || s2 < 0 || s1 > cont.getSize() || s2 > cont.getSize())return;
        if (s1 == s2)return;

        auto i1 = cont.getItem(s1).clone();
        auto i2 = cont.getItem(s2).clone();
        cont.removeItem(s1, 64);
        cont.removeItem(s2, 64);
        cont.setItem(s1, i2);
        cont.setItem(s2, i1);
    }

    bool switchItemToHandByName(SimulatedPlayer *player, const string &name) {
        int slot = -1;
        auto select = player->getSelectedItemSlot();
        auto *item = searchFirstItemInInvByName(player, slot, name);
        if (slot < 0)return false;
        swapTwoSlotInInventory(player->getInventory(), slot, select);
        return true;
    }


    bool switchItemToHandById(SimulatedPlayer *player, int itemId) {
        int slot = -1;
        auto select = player->getSelectedItemSlot();
        auto *item = searchFirstItemInInvById(player, slot, itemId);
        if (slot < 0)return false;
        swapTwoSlotInInventory(player->getInventory(), slot, select);
        return true;
    }

    //=========================序列化======================

    /**
      * 序列化背包内容到文件
      * @param cont
      * @param playerName
      */

    void writeInvToFile(Container &cont, const string &playerName) {
        if (!trapdoor::mod().getConfig().getBasicConfig().keepSimPlayerInv) return;
        const std::string path =
                "./plugins/trapdoor/sim/" + std::to_string(do_hash(playerName.c_str()));
        trapdoor::logger().debug("Inventory Write path is {}", path);

        nlohmann::json obj;
        auto array = nlohmann::json::array();
        for (auto i = 0; i < cont.getSize(); i++) {
            auto *item = cont.getSlot(i);
            std::string sNbt;
            if (item) {
                sNbt = item->getNbt()->toSNBT();
            }
            nlohmann::json j;
            j = {{"slot", i},
                 {"nbt",  sNbt}};
            array.push_back(j);
        }

        obj["inventory"] = array;
        obj["name"] = playerName;
        std::ofstream f(path);
        if (!f) {
            trapdoor::logger().error("Can not write file {} to disk", path);
            return;
        }

        f << obj;
        f.close();
    }


    void tryReadInvFromFile(Container &cont, const string &playerName) {
        if (!trapdoor::mod().getConfig().getBasicConfig().keepSimPlayerInv) return;
        const std::string path =
                "./plugins/trapdoor/sim/" + std::to_string(do_hash(playerName.c_str()));
        std::ifstream f(path);
        if (!f.is_open()) {
            return;
        }
        nlohmann::json obj;
        f >> obj;
        try {
            auto invInfo = obj["inventory"];
            for (auto &item: invInfo) {
                auto slot = item["slot"].get<int>();
                auto *it = ItemStack::create(CompoundTag::fromSNBT(item["nbt"]));
                cont.setItem(slot, *it);
            }
        } catch (const std::exception &) {
        }
    }

    ItemStack *getSelectItem(SimulatedPlayer *sim) {
        if (!sim)return nullptr;
        return sim->getInventory().getSlot(sim->getSelectedItemSlot());
    }

}
