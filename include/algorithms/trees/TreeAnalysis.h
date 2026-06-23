/*
 * File: TreeAnalysis.h
 * Description: Declares structural metrics over a read-only TargetTree.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "structures/TargetTree.h"

#include <cstddef>


// named container pds = Pluggy Data Structures
namespace pds
{
    std::size_t height(const TargetTree& tree);
    std::size_t nodeCount(const TargetTree& tree);
    std::size_t leafCount(const TargetTree& tree);
    bool isBalanced(const TargetTree& tree);
}
