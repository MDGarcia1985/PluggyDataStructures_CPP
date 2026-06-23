/*
 * File: GraphTraversal.cpp
 * Description: Implements graph BFS/DFS by adapting adjacency to the generic TraversalEngine.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/graphs/GraphTraversal.h"

#include "algorithms/common/TraversalEngine.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        /*
         * Purpose: Expose a node's neighbor ids to the generic traversal engine.
         * Design: Adapts weighted GraphEdge rows to plain ids without copying node values.
         * Workflow: Read the adjacency row for the id and collect each edge's target id in order.
         * Data Handoff: Supplies TraversalEngine with the neighbor ids it needs to expand.
         */
        std::vector<NodeId> neighborIds(const TargetGraph& graph, NodeId id)
        {
            std::vector<NodeId> ids;
            const std::vector<GraphEdge>& edges = graph.neighbors(id);
            ids.reserve(edges.size());
            for (const GraphEdge& edge : edges)
            {
                ids.push_back(edge.to);
            }
            return ids;
        }

        std::vector<Target> mapToTargets(const TargetGraph& graph, const std::vector<NodeId>& ids)
        {
            std::vector<Target> targets;
            targets.reserve(ids.size());
            for (const NodeId id : ids)
            {
                targets.push_back(graph.node(id));
            }
            return targets;
        }
    }

    /*
     * Purpose: Visit nodes in breadth-first order from a start id.
     * Design: Delegates ordering to TraversalEngine, preserving the original FIFO/enqueue-mark behavior.
     * Workflow: Build a neighbor provider, run the engine, and map visited ids back to Targets.
     * Data Handoff: Returns the visited Targets in discovery order.
     */
    std::vector<Target> breadthFirst(const TargetGraph& graph, std::size_t start)
    {
        const std::vector<NodeId> ids = breadthFirstIds(
            start,
            graph.nodeCount(),
            [&graph](NodeId id) { return neighborIds(graph, id); });
        return mapToTargets(graph, ids);
    }

    /*
     * Purpose: Visit nodes in depth-first order from a start id.
     * Design: Delegates ordering to TraversalEngine, which reverse-pushes neighbors to match prior order.
     * Workflow: Build a neighbor provider, run the engine, and map visited ids back to Targets.
     * Data Handoff: Returns the visited Targets in discovery order.
     */
    std::vector<Target> depthFirst(const TargetGraph& graph, std::size_t start)
    {
        const std::vector<NodeId> ids = depthFirstIds(
            start,
            graph.nodeCount(),
            [&graph](NodeId id) { return neighborIds(graph, id); });
        return mapToTargets(graph, ids);
    }
}
