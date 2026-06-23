/*
 * File: TreeSearch.cpp
 * Description: Implements BST key search, extremes, and root-to-node path queries.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/trees/TreeSearch.h"

#include "algorithms/common/KeyNormalization.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Find a stored Target by its first-field key.
     * Design: Descends the BST using normalized keys so lookups stay O(height) on typical data.
     * Workflow: Compare normalized keys at each node, moving left or right until matched or exhausted.
     * Data Handoff: Writes the matched Target into found and reports whether a match existed.
     */
    bool find(const TargetTree& tree, const std::string& key, Target& found)
    {
        return tree.findByKey(key, found);
    }

    /*
     * Purpose: Return the smallest Target by the tree's ordering.
     * Design: Follows left links because the leftmost node holds the minimum.
     * Workflow: Descend left until a node has no left child, then copy it out.
     * Data Handoff: Writes the minimum into found and reports whether the tree had any node.
     */
    bool minimum(const TargetTree& tree, Target& found)
    {
        return tree.minimum(found);
    }

    /*
     * Purpose: Return the largest Target by the tree's ordering.
     * Design: Follows right links because the rightmost node holds the maximum.
     * Workflow: Descend right until a node has no right child, then copy it out.
     * Data Handoff: Writes the maximum into found and reports whether the tree had any node.
     */
    bool maximum(const TargetTree& tree, Target& found)
    {
        return tree.maximum(found);
    }

    /*
     * Purpose: Return the root-to-node path of Targets for a matching key.
     * Design: Records each node visited on the descent so the search path is visible.
     * Workflow: Descend by normalized key, appending each node, until matched or exhausted.
     * Data Handoff: Returns the path on a match or an empty vector when the key is absent.
     */
    std::vector<Target> pathToNode(const TargetTree& tree, const std::string& key)
    {
        return tree.pathToKey(key);
    }
}
