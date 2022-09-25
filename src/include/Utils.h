#ifndef TRAPDOOR_UTILS_H
#define TRAPDOOR_UTILS_H
#include <mc/CompoundTag.hpp>
#include <string>
namespace trapdoor {
    std::string getNBTInfoFromPath(const std::unique_ptr<CompoundTag> &tag, const std::string &path,
                                   bool &success);

    std::string rmmc(const std::string &s);

    std::string i18ActorName(const std::string &name);

}  // namespace trapdoor

#endif