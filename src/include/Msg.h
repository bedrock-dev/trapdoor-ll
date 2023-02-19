//
// Created by xhy on 2020/8/24.
//

#ifndef TRAPDOOR_MSG_BUILDER_H
#define TRAPDOOR_MSG_BUILDER_H

#include <cstdarg>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "FMT/format.h"

#define PH_DARK_RED "§4{}§r"
#define PH_RED "§c{}§r"
#define PH_GOLD "§6{}§r"
#define PH_DARK_GREEN "§2{}§r"
#define PH_YELLOW "§e{}§r"
#define PH_GREEN "§a{}§r"
#define PH_AQUA "§b{}§r"
#define PH_BLUE "§9{}§r"
#define PH_LIGHT_PURPLE "§d{}§r"
#define PH_WHITE "§f{}§r"
#define PH_GRAY "§7{}§r"
#define PH_DOUB "§f{:.3lf}§r"
#define PH_NUM "§2{}§r"
#define PH_POS "§a{},{},{}§r"

namespace trapdoor {
    void broadcastMessage(const std::string &msg, int level = 0);

    // from stack overflow
    template<typename... Args>
    std::string format(const std::string &format, Args... args) {
        int size = snprintf(nullptr, 0, format.c_str(), args...) + 1;  // Extra space for '\0'
        if (size <= 0) return {};
        std::unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), size, format.c_str(), args...);
        return {buf.get(), buf.get() + size - 1};  // We don't want the '\0' inside
    }

    struct TVec3;
    struct TBlockPos;
    struct TAABB;

    class TextBuilder {
    public:
        static const uint8_t DARK_RED;
        static const uint8_t RED;
        static const uint8_t GOLD;
        static const uint8_t YELLOW;
        static const uint8_t GREEN;
        static const uint8_t DARK_GREEN;
        static const uint8_t AQUA;
        static const uint8_t BLUE;
        static const uint8_t LIGHT_PURPLE;
        static const uint8_t WHITE;
        static const uint8_t GRAY;
        static const uint8_t BLACK;

        static const uint8_t BOLD;
        static const uint8_t ITALIC;
        static const uint8_t RESET;

        TextBuilder &text(const std::string &s);

        template<typename... Args>
        TextBuilder &textF(const std::string &format, Args... args) {
            this->text(trapdoor::format(format, args...));
            return *this;
        }

        template<typename... Args>
        TextBuilder &sTextF(uint8_t style, const std::string &format, Args... args) {
            this->sText(style, trapdoor::format(format, args...));
            return *this;
        }

        template<typename... Args>
        TextBuilder &boldF(const std::string &format, Args... args) {
            this->sText(TextBuilder::BOLD, trapdoor::format(format, args...));
            return *this;
        }

        inline TextBuilder &endl() {
            this->messageBuffer.emplace_back("\n");
            return *this;
        }

        inline TextBuilder &removeEndl() {
            if ((!this->messageBuffer.empty() && !this->messageBuffer.back().empty()) &&
                this->messageBuffer.back().back() == '\n') {
                this->messageBuffer.back().pop_back();
            }
            return *this;
        }

        TextBuilder &sText(uint8_t style, const std::string &s);

        TextBuilder &pos(const TBlockPos &pos);

        template<typename T>
        TextBuilder &num(T x) {
            if (std::is_arithmetic<T>()) {
                if (std::is_floating_point<T>()) {
                    char buffer[32];
                    sprintf(buffer, "%.1f", (double) x);
                    std::string s(buffer);
                    sText(WHITE | BOLD, s);
                } else {
                    sText(WHITE | BOLD, std::to_string(x));
                }
            }
            return *this;
        }

        inline TextBuilder itemStyle(const std::pair<uint8_t, uint8_t> &pair) {
            this->itemColor = pair;
            return *this;
        }

        template<typename T>
        inline TextBuilder &item(const std::string &key, T value) {
            sText(GRAY, " - ")
                    .sTextF(WHITE, "%s: ", key.c_str())
                    .sTextF(WHITE | BOLD, "%s", fmt::format("{}", value).c_str())
                    .endl();
            return *this;
        }

        template<>
        inline TextBuilder &item(const std::string &key, bool value) {
            auto iColor = value ? GREEN : RED;
            sText(GRAY, " - ")
                    .sTextF(WHITE, "%s: ", key.c_str())
                    .sTextF(iColor | BOLD, "%s", fmt::format("{}", value).c_str())
                    .endl();
            return *this;
        }

        TextBuilder &vec3(const TVec3 &vec3);

        TextBuilder &aabb(const TAABB &aabb);

        inline void clear() { this->messageBuffer.clear(); }

        std::string get();

        void broadcast();

        void operator+=(const std::string &str) { this->text(str); }

    private:
        std::pair<uint8_t, uint8_t> itemColor{WHITE, GREEN};
        const static std::map<uint8_t, std::string> STYLE_MAP;
        std::vector<std::string> messageBuffer;
    };

    typedef TextBuilder TB;
}  // namespace trapdoor
#endif  // TRAPDOOR_MSG_BUILDER_H
