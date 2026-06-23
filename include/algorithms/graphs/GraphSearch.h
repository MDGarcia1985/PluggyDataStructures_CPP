/*
 * File: GraphSearch.h
 * Description: Declares reachability and unweighted shortest-path queries over a TargetGraph.
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
    bool pathExists(const TargetGraph& graph, std::size_t from, std::size_t to);
    std::vector<Target> shortestPathUnweighted(const TargetGraph& graph, std::size_t from, std::size_t to);
}
