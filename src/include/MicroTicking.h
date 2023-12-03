//
// Created by xhy on 2023/12/3.
//

#ifndef MICROTICKING_H
#define MICROTICKING_H
#include <Msg.h>

#include <array>
#include <mc/BlockPos.hpp>
#include <vector>

#include "TAABB.h"
#include "TBlockPos.h"
#include "TVec3.h"

namespace trapdoor {
    class MicroTicking {
       public:
        void tick();

        void tickChunk(const TBlockPos2& chunkPos, int dimension);

        [[nodiscard]] std::vector<std::vector<int>> getTickingOrder(const BlockPos& pos, int dim,
                                                                    int radius) const;

        inline const std::vector<trapdoor::TBlockPos2>& tickingList(int dim) const {
            return cached_world_ticking_list_[dim];
        }

       private:
        std::array<std::vector<trapdoor::TBlockPos2>, 3> current_world_ticking_list_;
        std::array<std::vector<trapdoor::TBlockPos2>, 3> cached_world_ticking_list_;
    };
}  // namespace trapdoor
#endif  // MICROTICKING_H
