//
// Created by xhy on 2022/6/20.
//

#include "TrAPI.h"

#include "HookAPI.h"
namespace trapdoor {
    //! TODO
    std::string getBiomeName(Biome * biome) {
        if (!biome) return "";
        // return dAccess<std::string, 8>(biome);
        return biome->getName();
    }

    // !CircuitSystem::updateDependencies
    CircuitSceneGraph& getCircuitSceneGraph(CircuitSystem * system) {
        return dAccess<CircuitSceneGraph, 8>(system);
    }

}  // namespace trapdoor
