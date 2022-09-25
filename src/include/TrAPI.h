//
// Created by xhy on 2022/6/20.
//

#ifndef TRAPDOOR_TRAPI_H
#define TRAPDOOR_TRAPI_H

#include <mc/Biome.hpp>
#include <mc/CircuitSceneGraph.hpp>
#include <mc/CircuitSystem.hpp>
#include <string>
namespace trapdoor {
    std::string getBiomeName(Biome *biome);

    CircuitSceneGraph &getCircuitSceneGraph(CircuitSystem *system);
}  // namespace trapdoor

#endif  // TRAPDOOR_TRAPI_H
