//
// Created by xhy on 2023/3/11.
//

#ifndef TRAPDOOR_SIMULATEAPIWRAPPER_H
#define TRAPDOOR_SIMULATEAPIWRAPPER_H

#include "Utils.h"
#include <mc/ItemStack.hpp>
#include <mc/SimpleContainer.hpp>
#include <mc/SimulatedPlayer.hpp>

namespace trapdoor::bot {


//根据条件搜索某个物品再某个背包内的位置
    ItemStack *
    searchFirstItemInInv(SimulatedPlayer *sim, int &slot, const std::function<bool(const ItemStack *)> &filter);

//根据类型名字搜索某个物品在背包的位置
    ItemStack *searchFirstItemInInvByName(SimulatedPlayer *sim, int &slot, const std::string &name);

    //根据ID搜索某个物品在背包的位置
    ItemStack *searchFirstItemInInvById(SimulatedPlayer *sim, int &slot, int id);


    void swapTwoSlotInInventory(Container &cont, int s1, int s2);

    bool switchItemToHandByName(SimulatedPlayer *player, const std::string &name);

    bool switchItemToHandById(SimulatedPlayer *player, int itemId);

    ItemStack *getSelectItem(SimulatedPlayer *sim);


    void writeInvToFile(Container &cont, const std::string &playerName);

    void tryReadInvFromFile(Container &cont, const std::string &playerName);


}

#endif //TRAPDOOR_SIMULATEAPIWRAPPER_H
