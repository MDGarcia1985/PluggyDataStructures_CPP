/*
 * File: GraphTraversal.h
 * Description: Declares breadth-first and depth-first traversals over a read-only TargetGraph.
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
    std::vector<Target> breadthFirst(const TargetGraph& graph, std::size_t start);
    std::vector<Target> depthFirst(const TargetGraph& graph, std::size_t start);
}
