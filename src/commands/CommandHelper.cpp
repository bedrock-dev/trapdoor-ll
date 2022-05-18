//
// Created by xhy on 2022/5/18.
//
#include "CommandHelper.h"

#include "DynamicCommandAPI.h"

namespace tr {

    ActionResult::ActionResult(std::string m, bool su)
        : msg(std::move(m)), success(su) {}
    void ActionResult::SendTo(CommandOutput &output) const {
        success ? output.success(msg) : output.error(msg);
    }
}  // namespace tr