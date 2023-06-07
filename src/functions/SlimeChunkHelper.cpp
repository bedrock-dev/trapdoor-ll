#include "SlimeChunkHelper.h"

#include "Particle.h"
#include "TrapdoorMod.h"

namespace trapdoor {
    void SlimeChunkHelper::HeavyTick() {
        static int gt = 0;
        constexpr int frequency = 100;
        if (!this->showSlime) return;
        if (gt % frequency == 0) {
            this->updateChunkPosList();
            this->draw();
        }
        gt = (gt + 1) % frequency;
    }

    void SlimeChunkHelper::updateChunkPosList() {
        if (!this->showSlime) return;
        this->posList.clear();
        Global<Level>->forEachPlayer([&](Player &player) {
            if (player.getDimensionId() == 0) {  // 遍历主世界玩家
                int num = 0;
                auto playerPos = player.getPosition();
                trapdoor::TBlockPos tPos(playerPos.x, playerPos.y, playerPos.z);
                auto playerChunkPos = tPos.toChunkPos();
                auto pos = playerChunkPos;
                for (int i = -showRadius; i < showRadius; i++) {
                    for (int j = -showRadius; j < showRadius; j++) {
                        pos.x = playerChunkPos.x + i;
                        pos.z = playerChunkPos.z + j;
                        if (pos.isSlimeChunk()) {
                            ++num;
                            this->posList.insert(pos);
                        }
                    }
                }
            }
            return true;
        });
    }

    ActionResult SlimeChunkHelper::draw() {
        for (const auto i : this->posList) trapdoor::spawnSlimeChunkParticle(i);
        return SuccessMsg("slime.info.refresh");
    }

    ActionResult SlimeChunkHelper::setRadius(int r) {
        this->showRadius = r;
        return {fmt::format(tr("slime.info.radius"), r), true};
    }
}  // namespace trapdoor
