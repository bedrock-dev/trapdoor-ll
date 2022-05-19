
#include "Msg.h"

#include <MC/Level.hpp>

#include "FMT/core.h"


namespace tr {

    void BroadcastMessage(const std::string &msg, int level) {
        if (level == 0) {
            Level::broadcastText(msg, TextType::RAW);
        } else if (level < 0) {
            Level::broadcastText(fmt::format(PH_GRAY, msg), TextType::RAW);
        } else if (level > 0) {
            Level::broadcastText(fmt::format(PH_GREEN, msg), TextType::RAW);
        }
    }
}  // namespace tr
