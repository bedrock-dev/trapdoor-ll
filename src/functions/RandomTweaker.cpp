//
// Created by xhy on 2022/10/5.
//

#include "RandomTweaker.h"

#include "TrapdoorMod.h"
namespace trapdoor {

    namespace {

        enum RandType { Normal, Gauss, UnknownRand };

        enum RandomStatus { Reset, Range, Fixed, Scale };

        struct RandomRtInfo {
            RandType type = UnknownRand;
            RandomStatus status = Reset;
            int fixedValue = 0;
            int rangeMin = 0;
            int rangeMax = INT_MAX;
            float scaleValue = 1.0f;
        };

        // global value

        RandomRtInfo& getRandRtInfo(const std::string& type) {
            static RandomRtInfo normalRtInfo, gaussRtInfo, unknownInfo;
            normalRtInfo.type = Normal;
            gaussRtInfo.type = Gauss;
            unknownInfo.type = UnknownRand;
            if (type == "gauss") {
                return gaussRtInfo;
            } else if (type == "normal") {
                return normalRtInfo;
            }
            return unknownInfo;
        }
    }  // namespace

    ActionResult trapdoor::tweakFixedRandomNumber(const std::string& type, int value) {
        auto& info = getRandRtInfo(type);
        if (info.type == UnknownRand) {
            return ErrorMsg("Unknown random type");
        }
        if (value < 0) {
            return ErrorRange("fixed random value", 0, INT_MAX);
        }

        trapdoor::logger().debug("Tweak {}  random with fixed value {}", type, value);
        info.status = Fixed;
        info.fixedValue = value;
        return {"Success", true};
    }
}  // namespace trapdoor

// THook(__int64, "?_genRandInt32@Random@Core@@AEAAIXZ", void* self) {
//     auto& rtInfo = trapdoor::getRandRtInfo("normal");
//     if (rtInfo.type == trapdoor::Normal && rtInfo.status == trapdoor::Fixed) {
//         return rtInfo.fixedValue;
//     }
//     return original(self);
// }