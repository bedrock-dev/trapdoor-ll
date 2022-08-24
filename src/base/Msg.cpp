#include "Msg.h"

#include <MC/Level.hpp>

#include "FMT/core.h"
#include "TAABB.h"
#include "TBlockPos.h"
#include "TVec3.h"

namespace trapdoor {
    void broadcastMessage(const std::string &msg, int level) {
        auto fmtMsg = msg;
        if (level < 0) {
            fmtMsg = fmt::format(PH_GRAY, msg);
        } else if (level > 0) {
            fmtMsg = fmt::format(PH_GREEN, msg);
        }
        Level::broadcastText(fmtMsg, TextType::RAW);
        trapdoor::logger().info(fmtMsg);
    }

    const uint8_t TextBuilder::BLACK = 0x0;
    const uint8_t TextBuilder::DARK_GREEN = 0x2;
    const uint8_t TextBuilder::DARK_RED = 0x4;
    const uint8_t TextBuilder::GOLD = 0x6;
    const uint8_t TextBuilder::GRAY = 0x7;
    const uint8_t TextBuilder::BLUE = 0x9;
    const uint8_t TextBuilder::GREEN = 0xa;
    const uint8_t TextBuilder::AQUA = 0xb;
    const uint8_t TextBuilder::RED = 0xc;
    const uint8_t TextBuilder::LIGHT_PURPLE = 0xd;
    const uint8_t TextBuilder::YELLOW = 0xe;
    const uint8_t TextBuilder::WHITE = 0xf;

    const uint8_t TextBuilder::BOLD = 0x10;
    const uint8_t TextBuilder::ITALIC = 0x20;
    const uint8_t TextBuilder::RESET = 0x30;

    const std::map<uint8_t, std::string> TextBuilder::STYLE_MAP = {
        {BLACK, "§0"},  {DARK_GREEN, "§2"}, {DARK_RED, "§4"}, {GOLD, "§6"},   {GRAY, "§7"},
        {BLUE, "§9"},   {GREEN, "§a"},      {AQUA, "§b"},     {RED, "§c"},    {LIGHT_PURPLE, "§d"},
        {YELLOW, "§e"}, {WHITE, "§f"},      {BOLD, "§l"},     {ITALIC, "§o"}, {RESET, "§r"}};

    TextBuilder &TextBuilder::text(const std::string &s) {
        messageBuffer.emplace_back(s);
        return *this;
    }

    TextBuilder &TextBuilder::pos(const TBlockPos &pos) {
        return this->sText(WHITE | BOLD, pos.toString());
    }

    TextBuilder &TextBuilder::vec3(const TVec3 &vec3) {
        return this->sText(WHITE | BOLD, vec3.toString());
    }

    TextBuilder &TextBuilder::aabb(const TAABB &aabb) {
        auto str = "[" + aabb.p1.toString() + "," + aabb.p2.toString() + "]";
        return this->sText(WHITE, str);
    }

    std::string TextBuilder::get() {
        std::string s;
        for (const auto &i : messageBuffer) {
            s.append(i);
        }
        return s;
    }

    TextBuilder &TextBuilder::sText(uint8_t style, const std::string &s) {
        auto fontColor = style & 0x0fu;
        auto fontStyle = style & 0xf0u;
        auto iter = STYLE_MAP.find(fontColor);
        if (iter != STYLE_MAP.end()) messageBuffer.push_back(iter->second);
        iter = STYLE_MAP.find(fontStyle);
        if (iter != STYLE_MAP.end() && fontStyle != 0x0) messageBuffer.push_back(iter->second);
        messageBuffer.push_back(s);
        messageBuffer.push_back(STYLE_MAP.at(RESET));
        return *this;
    }

    void TextBuilder::broadcast() { trapdoor::broadcastMessage(this->get()); }

}  // namespace trapdoor
