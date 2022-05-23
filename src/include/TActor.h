#ifndef TRAPDOOR_TACTOR
#define TRAPDOOR_TACTOR
#include <cstdint>

#include "TBlockPos.h"

namespace tr {

    struct TActorUniqueID {
        int64_t uid;

        bool operator==(const TActorUniqueID &u) const {
            return this->uid == u.uid;
        }
    };

    struct TActorUniqueIDHash {
        static uint64_t mceHaseAccLong(uint64_t a1, uint64_t a2) {
            return ((a1 >> 2) + (a1 << 6) + a2 + 2654435769u) ^ a1;
        }

        static uint64_t mceHsaLongLong(uint64_t a1, uint64_t a2) {
            auto x = mceHaseAccLong(0, a1);
            return mceHaseAccLong(x, a2);
        }

        size_t operator()(const TActorUniqueID &key) const {
            return mceHsaLongLong(key.uid >> 32, key.uid);
        }
    };

    struct TDwellerData {
        uint64_t tick;
        TBlockPos pos;
    };

}  // namespace tr
#endif
