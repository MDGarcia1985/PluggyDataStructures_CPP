/*
 * File: ShortestPath.h
 * Description: Declares a minimal weighted shortest-path (Dijkstra) query over a TargetGraph.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Target.h"
#include "structures/TargetGraph.h"

#include <cstddef>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    struct ShortestPathResult
    {
        bool reachable = false;
        double distance = 0.0;
        std::vector<Target> path;
    };

    ShortestPathResult shortestPath(const TargetGraph& graph, std::size_t from, std::size_t to);
}
