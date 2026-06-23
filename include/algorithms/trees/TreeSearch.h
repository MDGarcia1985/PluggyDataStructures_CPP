/*
 * File: TreeSearch.h
 * Description: Declares key search, minimum, maximum, and path queries over a TargetTree.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Target.h"
#include "structures/TargetTree.h"

#include <string>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    bool find(const TargetTree& tree, const std::string& key, Target& found);
    bool minimum(const TargetTree& tree, Target& found);
    bool maximum(const TargetTree& tree, Target& found);
    std::vector<Target> pathToNode(const TargetTree& tree, const std::string& key);
}
