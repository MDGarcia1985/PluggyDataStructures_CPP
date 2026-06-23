/*
 * File: CollisionAnalysis.h
 * Description: Declares shared collision and probe metrics over hashing strategies.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "algorithms/hashing/LinearProbing.h"
#include "algorithms/hashing/SeparateChaining.h"

#include <cstddef>


// named container pds = Pluggy Data Structures
namespace pds
{
    std::size_t collisionCount(const SeparateChaining& table);
    std::size_t longestChain(const SeparateChaining& table);
    double loadFactor(const SeparateChaining& table);

    std::size_t collisionCount(const LinearProbing& table);
    std::size_t longestCluster(const LinearProbing& table);
    double averageProbeLength(const LinearProbing& table);
    double loadFactor(const LinearProbing& table);
}
