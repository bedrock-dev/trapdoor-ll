#include "Utils.h"

#include "TrapdoorMod.h"
namespace tr {

    std::string rmmc(const std::string& s) { return s.find("minecraft:") == 0 ? s.substr(10) : s; }
    std::unique_ptr<CompoundTag> getTagFromPath(const std::unique_ptr<CompoundTag>& tag,
                                                const string& path) {
        if (!tag) return nullptr;
        if (path.empty()) return tag->clone();
        std::string curPath;
        auto pos = path.find('.');
        if (pos == string::npos) {
            curPath = path;
        } else {
            curPath = path.substr(0, pos);
        }

        tr::logger().debug("path {}", curPath);

        if (pos == std::string::npos) {
            return tag->clone();
        } else {
            return getTagFromPath(tag, path.substr(pos + 1));
        }
    }

}  // namespace tr
