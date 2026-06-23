/*
 * File: TreeTraversal.cpp
 * Description: Implements in-, pre-, post-, and level-order traversals over a TargetTree.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/trees/TreeTraversal.h"

// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        std::vector<Target> collect(const TargetTree& tree, TreeVisitOrder order)
        {
            std::vector<Target> output;
            output.reserve(tree.size());
            tree.visit(order, [&output](const Target& target) { output.push_back(target); });
            return output;
        }
    }

    /*
     * Purpose: Return Targets in ascending (in-order) sequence.
     * Design: Reads the tree through its const root so storage stays owned by TargetTree.
     * Workflow: Recursively visit left, node, then right, collecting values.
     * Data Handoff: Returns a sorted vector snapshot for display or tests.
     */
    std::vector<Target> inOrder(const TargetTree& tree)
    {
        return collect(tree, TreeVisitOrder::InOrder);
    }

    /*
     * Purpose: Return Targets in pre-order (node, left, right) sequence.
     * Design: Reveals the tree's root-first structure for teaching and verification.
     * Workflow: Recursively visit node, left, then right, collecting values.
     * Data Handoff: Returns a vector snapshot for display or tests.
     */
    std::vector<Target> preOrder(const TargetTree& tree)
    {
        return collect(tree, TreeVisitOrder::PreOrder);
    }

    /*
     * Purpose: Return Targets in post-order (left, right, node) sequence.
     * Design: Reveals the children-first structure used by safe deletion.
     * Workflow: Recursively visit left, right, then node, collecting values.
     * Data Handoff: Returns a vector snapshot for display or tests.
     */
    std::vector<Target> postOrder(const TargetTree& tree)
    {
        return collect(tree, TreeVisitOrder::PostOrder);
    }

    /*
     * Purpose: Return Targets in breadth-first (level-order) sequence.
     * Design: Uses a FIFO queue of nodes to visit each depth before the next.
     * Workflow: Enqueue the root, then repeatedly dequeue a node and enqueue its children.
     * Data Handoff: Returns a vector snapshot for display or tests.
     */
    std::vector<Target> levelOrder(const TargetTree& tree)
    {
        return collect(tree, TreeVisitOrder::LevelOrder);
    }
}
