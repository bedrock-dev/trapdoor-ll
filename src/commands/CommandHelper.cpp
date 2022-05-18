//
// Created by xhy on 2022/5/18.
//
#include "CommandHelper.h"
#include "DynamicCommandAPI.h"

namespace tr {

    CommandResult::CommandResult(std::string m, bool su) : msg(std::move(m)), success(su) {}
    void CommandResult::Send(CommandOutput &output) const {
        success ? output.success(msg) :
        output.error(msg);
    }
}