#include "HUDHelper.h"

#include <MC/Level.hpp>
#include <MC/Player.hpp>

#include "CommandHelper.h"
#include "MCTick.h"
#include "PlayerInfoAPI.h"

namespace tr {

    namespace {
        HUDInfoType getTypeFromString(const std::string& str) {
            if (str == "base") return HUDInfoType::Base;
            if (str == "counter") return HUDInfoType::Counter;
            if (str == "village") return HUDInfoType::Vill;
            if (str == "mspt") return HUDInfoType::Mspt;
            return HUDInfoType::Unknown;
        }

    }  // namespace
    void HUDHelper::tick() {
        static int refresh_time = 0;
        refresh_time = (refresh_time + 1) % 30;
        if (refresh_time != 1) return;
        for (auto& info : this->playerInfos) {
            auto* p = Global<Level>->getPlayer(info.first);
            if (p && info.second.enbale) {
                std::string s;
                auto& cfg = info.second.config;
                if (cfg[HUDInfoType::Base]) {
                    s += "Base\n";
                }
                if (cfg[HUDInfoType::Mspt]) {
                    s += "MSPT\n";
                }
                if (cfg[HUDInfoType::Vill]) {
                    s += "Village\n";
                }
                if (cfg[HUDInfoType::Counter]) {
                    s += "Counter\n";
                }
                p->sendText(s, TextType::TIP);
            }
        }
    }

    ActionResult HUDHelper::modifyPlayerInfo(const std::string& playerName,
                                             const std::string& item, int op) {
        auto type = getTypeFromString(item);
        if (type == HUDInfoType::Unknown) {
            return {"Unknown type", false};
        }
        this->playerInfos[playerName].config[type] = op;
        return {"success", true};
    }

    ActionResult HUDHelper::setAblePlayer(const std::string& playerName,
                                          bool able) {
        this->playerInfos[playerName].enbale = able;
        return {"success", true};
    }
}  // namespace tr
