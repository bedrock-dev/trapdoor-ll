#include "VillageHelper.h"

#include <MC/Dimension.hpp>
#include <MC/Level.hpp>
#include <MC/Village.hpp>
#include <MC/VillageManager.hpp>

#include "GlobalServiceAPI.h"

void f() {
    auto dim = Level::getDimension(0);
    auto &vm = dim->getVillageManager();
    Village *village;
    village->debugDraw();
}