//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_COMMANDHELPER_H
#define TRAPDOOR_COMMANDHELPER_H

#include <string>
#include <tuple>
#include <utility>

class CommandOutput;
namespace tr {
    struct ActionResult {
        std::string msg;
        bool success;

        ActionResult(std::string m, bool su);

        void SendTo(CommandOutput &output) const;
    };

    ActionResult ErrorMsg(const std::string &msg);

    void SetupTickCommand();

    void SetupProfCommand();

    void SetupVillageCommand();

    void SetupLogCommand();

    void SetupTestCommand();

    void SetupSpawnCommand();

    void SetupDataCommand();

    void SetupHsaCommand();

}  // namespace tr

#endif  // TRAPDOOR_COMMANDHELPER_H
