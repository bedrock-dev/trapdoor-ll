#include <MC/SimulatedPlayer.hpp>
#include <gsl/gsl>

#include "SimPlayerHelper.h"

namespace tr {

    /*


        MCAPI static class SimulatedPlayer * create(std::string const &, class
       BlockPos const &, class AutomaticID<class Dimension, int>, class
       gsl::not_null<class Bedrock::NonOwnerPointer<class
       ServerNetworkHandler>>);

    */
    ActionResult SimPlayerManager::addPlayer(const std::string& name,
                                             const BlockPos& p, int dimID) {
        SimulatedPlayer::create(name, p, dimID);
        return {"~", true};
    }
}  // namespace tr