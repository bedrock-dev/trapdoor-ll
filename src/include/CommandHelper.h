//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_COMMANDHELPER_H
#define TRAPDOOR_COMMANDHELPER_H

#include <tuple>
#include <utility>
#include <string>

class CommandOutput;
namespace tr {
    struct CommandResult {
        std::string msg;
        bool success;

        CommandResult(std::string m, bool su);

        void Send(CommandOutput &output) const;
    };

    void SetupTickCommand();

    void SetupProfCommand();

    void SetupVillageCommand();

    void SetupLogCommand();

}


#endif //TRAPDOOR_COMMANDHELPER_H
