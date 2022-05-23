#include "SpawnHelper.h"

#include <MC/Level.hpp>
#include <MC/Spawner.hpp>

#include "GlobalServiceAPI.h"

namespace tr {
    ActionResult PrintCap() {
        auto& sp = Global<Level>->getSpawner();

        return {"", true};
    }
}  // namespace tr
