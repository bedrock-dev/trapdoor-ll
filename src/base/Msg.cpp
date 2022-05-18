
#include "Msg.h"

#include <MC/Level.hpp>

#include "FMT/core.h"

namespace tr {

    void BroadcastMessage(const std::string& msg, int level) {
        if (level == 0) {
            Level::broadcastText(msg, static_cast<TextType>(0));

        } else if (level < 0) {
            Level::broadcastText(fmt::format(PH_GRAY, msg),
                                 static_cast<TextType>(0));
        } else if (level > 0) {
            Level::broadcastText(fmt::format(PH_GREEN, msg),
                                 static_cast<TextType>(0));
        }
    }

}  // namespace tr
