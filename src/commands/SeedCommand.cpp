//
// Created by xhy on 2022/7/15.
//
#include <MC/Level.hpp>
#include <MC/LevelData.hpp>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "Global.h"
#include "Msg.h"
namespace trapdoor {

    namespace {

        ActionResult getLevelSeed() {
            auto seed = Global<Level>->getSeed();
            trapdoor::TextBuilder builder;
            builder.text("Level seed is ").num(seed);
            return {builder.get(), true};
        }
        ActionResult getPlayerEnchantSeed(Player *p) {
            if (!p) return ErrorPlayerNeed();
            auto seed = p->getEnchantmentSeed();
            trapdoor::TextBuilder builder;
            builder.textF("Player %s's enchant seed is ", p->getRealName().c_str()).num(seed);
            return {builder.get(), true};
        }
    }  // namespace

    void setup_seedCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        auto command = DynamicCommand::createCommand("seed", "print seed",
                                                     static_cast<CommandPermissionLevel>(level));
        auto &seedItemEnum = command->setEnum("showSubCommand", {"enchant", "level"});

        command->mandatory("seed", ParamType::Enum, seedItemEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->addOverload({seedItemEnum});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["seed"].getRaw<std::string>().c_str())) {
                case do_hash("level"):
                    getLevelSeed().sendTo(output);
                    break;
                case do_hash("enchant"):
                    getPlayerEnchantSeed(origin.getPlayer()).sendTo(output);
                    break;
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor