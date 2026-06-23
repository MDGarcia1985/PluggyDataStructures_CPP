/*
 * File: TreeAnalysis.cpp
 * Description: Implements height, node count, leaf count, and balance checks for a TargetTree.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/trees/TreeAnalysis.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Report the height of the tree.
     * Design: Defines an empty tree as height zero and a single node as height one.
     * Workflow: Recursively measure the deeper subtree from the root.
     * Data Handoff: Gives callers a balance indicator for the current shape.
     */
    std::size_t height(const TargetTree& tree)
    {
        return tree.height();
    }

    /*
     * Purpose: Count the nodes reachable from the root.
     * Design: Recomputes from structure rather than trusting a cached count.
     * Workflow: Recursively sum one plus each subtree's node count.
     * Data Handoff: Returns the structural node total to callers and tests.
     */
    std::size_t nodeCount(const TargetTree& tree)
    {
        return tree.size();
    }

    /*
     * Purpose: Count the leaf nodes in the tree.
     * Design: Treats a node with no children as a leaf.
     * Workflow: Recursively sum leaves in both subtrees.
     * Data Handoff: Returns the leaf total to callers and tests.
     */
    std::size_t leafCount(const TargetTree& tree)
    {
        return tree.leafCount();
    }

    /*
     * Purpose: Report whether the tree is height-balanced.
     * Design: Considers a tree balanced when every node's subtree heights differ by at most one.
     * Workflow: Recursively verify balance while threading subtree heights upward.
     * Data Handoff: Returns a single boolean balance verdict.
     */
    bool isBalanced(const TargetTree& tree)
    {
        return tree.isBalanced();
    }
}
