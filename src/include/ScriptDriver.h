//
// Created by xhy on 2023/3/10.
//

#ifndef TRAPDOOR_SCRIPTDRIVER_H
#define TRAPDOOR_SCRIPTDRIVER_H

#include "sol.hpp"
#include <mc/SimulatedPlayer.hpp>
#include "Msg.h"

namespace trapdoor {

    class ScriptDriver {

    public:
        //BOT API
        struct Bot {
            void say(const std::string &msg) const {
                trapdoor::broadcastMessage(fmt::format(" <{}> {}", this->player->getRealName(), msg));
            }

            void jump() const {
                player->simulateJump();
            }

            void moveto(float px, float py, float pz) const {
                player->simulateMoveToLocation({px, py, pz}, 1.0f);
            }

            void update() {
                auto pos = player->getPos();
                x = pos.x;
                y = pos.y;
                z = pos.z;
            }

            SimulatedPlayer *player{nullptr};
            float x;
            float y;
            float z;
        };

        bool init(const std::string &fileName, SimulatedPlayer *p);

        void tick();

        void stop();

        ScriptDriver() = default;

    private:
        sol::state engine; //engine
        Bot bot; //for data binding
        bool running{false};
    };
}

#endif //TRAPDOOR_SCRIPTDRIVER_H
