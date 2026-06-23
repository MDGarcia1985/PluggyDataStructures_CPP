/*
 * File: TreeTraversal.h
 * Description: Declares pure traversal orders over a read-only TargetTree.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Target.h"
#include "structures/TargetTree.h"

#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    std::vector<Target> inOrder(const TargetTree& tree);
    std::vector<Target> preOrder(const TargetTree& tree);
    std::vector<Target> postOrder(const TargetTree& tree);
    std::vector<Target> levelOrder(const TargetTree& tree);
}
