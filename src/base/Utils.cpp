#include "Utils.h"

#include <MC/I18n.hpp>
#include <MC/ListTag.hpp>
#include <algorithm>

#include "TrapdoorMod.h"
namespace trapdoor {
    namespace {

        struct NBTPath {
            int type;
            const std::string key;
            int index;
        };

        void parsePath(const std::string& pathStr, std::vector<NBTPath>& paths) {
            std::string buff;
            for (auto c : pathStr) {
                if (c == '.') {
                    if (buff.size() >= 3 && *buff.begin() == '[' && buff.back() == ']') {
                        auto index = strtol(buff.c_str() + 1, nullptr, 10);
                        NBTPath path{1, "", index};
                        paths.push_back(path);
                    } else if (!buff.empty()) {
                        NBTPath path{0, buff, -1};
                        paths.push_back(path);
                    }
                    buff.clear();
                } else {
                    buff += c;
                }
            }
        }
    }  // namespace

    std::string rmmc(const std::string& s) { return s.find("minecraft:") == 0 ? s.substr(10) : s; }
    std::string getNBTInfoFromPath(const std::unique_ptr<CompoundTag>& tag, const string& path,
                                   bool& success) {
        success = false;
        std::vector<NBTPath> paths;
        Tag* cur = tag.get();
        parsePath(path + ".", paths);
        for (auto& p : paths) {
            if (p.type == 0) {
                auto* comp = cur->asCompoundTag();
                if (!comp) {
                    return "Invalid path";
                }
                cur = cur->asCompoundTag()->get(p.key);
                if (!cur) {
                    return "No key named " + p.key;
                }
            } else {
                auto* list = cur->asListTag();
                if (!list) {
                    return "Invalid path";
                }
                auto idx = p.index < 0 ? list->size() + p.index : p.index;
                if (idx < 0 || idx >= list->size()) {
                    return "Index " + std::to_string(p.index) + " out of range, it should within " +
                           std::to_string(-list->size()) + " to " +
                           std::to_string(list->size() - 1);
                }
                cur = list->get(idx);
                if (!cur) {
                    return "Invalid path";
                }
            }
        }

        success = true;
        return cur->toJson(4);
    }
    std::string i18ActorName(const string& name) {
        return I18n::get("entity." + rmmc(name) + ".name");
    }
}  // namespace trapdoor
