#include "HUDHelper.h"

#include <MC/Level.hpp>
#include <MC/Player.hpp>

#include "CommandHelper.h"
#include "MCTick.h"
#include "PlayerInfoAPI.h"

namespace tr {
    void HUDHelper::tick() {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 30;
        if (refresh_time != 1) return;
        for (auto &info : this->playerInfos) {
        }
        // Player *p = Global<Level>->getPlayer("hhhxiao");

        // if (p) {
        //     p->sendText(PrintMspt().msg, TextType::TIP);
        // }
    }
}  // namespace tr
