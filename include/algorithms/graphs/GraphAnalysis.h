/*
 * File: GraphAnalysis.h
 * Description: Declares degree, connected-component, and cycle queries over a TargetGraph.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "structures/TargetGraph.h"

#include <cstddef>


// named container pds = Pluggy Data Structures
namespace pds
{
    std::size_t degree(const TargetGraph& graph, std::size_t id);
    std::size_t connectedComponents(const TargetGraph& graph);
    bool hasCycle(const TargetGraph& graph);
}
