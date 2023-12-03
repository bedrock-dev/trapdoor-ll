//
// Created by xhy on 2023/12/3.
//

#include "MicroTicking.h"
namespace trapdoor {
    void MicroTicking::tick() {
        this->cached_world_ticking_list_ = this->current_world_ticking_list_;
        for (int i = 0; i < 3; i++) {
            this->current_world_ticking_list_[i].clear();
        }
    }
    void MicroTicking::tickChunk(const TBlockPos2& chunkPos, int dimension) {
        this->current_world_ticking_list_[dimension].push_back(chunkPos);
    }
}  // namespace trapdoor