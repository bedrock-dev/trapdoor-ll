#ifndef TRAPDOOR_HSA_HELPER
#define TRAPDOOR_HSA_HELPER

#include <set>

#include "CommandHelper.h"
#include "TBlockPos.h"


namespace tr {

    enum StructureType {
        SwampHut,
        OceanMonument,
        PillagerOutpost,
        NetherFortress
    };

    struct HsaInfo {
        StructureType type = PillagerOutpost;
        TBoundingBox bb{};
        int dimensionID = 0;

        bool operator<(const HsaInfo &rhs) const;
    };

    class HsaManager {
        bool showHsa = false;
        std::set<HsaInfo> hsaList;

       public:
        inline void insert(HsaInfo info) { this->hsaList.insert(info); }

        void HeavyTick();

        ActionResult place();

        inline ActionResult clear() {
            auto num = this->hsaList.size();
            this->hsaList.clear();
            return {std::to_string(num), true};
        }

        inline ActionResult ShowHsa(bool show) {
            this->showHsa = show;
            return {"~", true};
        }
    };
}  // namespace tr

#endif
