#ifndef TRAPDOOR_UTILS_H
#define TRAPDOOR_UTILS_H
#include <MC/CompoundTag.hpp>
#include <string>
namespace tr {

    CompoundTag *getCompoundFromPath(CompoundTag *tag, const std::string &path);

    std::unique_ptr<CompoundTag> getTagFromPath(const std::unique_ptr<CompoundTag> &tag,
                                                const std::string &path);

    std::string rmmc(const std::string &s);
}  // namespace tr

#endif