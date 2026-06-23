/*
 * File: GraphSearch.cpp
 * Description: Implements reachability and unweighted shortest path using breadth-first search.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/graphs/GraphSearch.h"

#include <queue>


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Report whether a directed path of edges connects two nodes.
     * Design: Runs a bounded breadth-first search following stored adjacency edges.
     * Workflow: Validate ids, BFS from the source, and report whether the target is reached.
     * Data Handoff: Receives node ids and returns a reachability verdict.
     */
    bool pathExists(const TargetGraph& graph, std::size_t from, std::size_t to)
    {
        const std::size_t nodeCount = graph.nodeCount();
        if (from >= nodeCount || to >= nodeCount)
        {
            return false;
        }
        if (from == to)
        {
            return true;
        }

        std::vector<bool> visited(nodeCount, false);
        std::queue<std::size_t> pending;
        visited[from] = true;
        pending.push(from);

        while (!pending.empty())
        {
            const std::size_t current = pending.front();
            pending.pop();

            for (const GraphEdge& edge : graph.neighbors(current))
            {
                if (edge.to == to)
                {
                    return true;
                }
                if (edge.to < nodeCount && !visited[edge.to])
                {
                    visited[edge.to] = true;
                    pending.push(edge.to);
                }
            }
        }

        return false;
    }

    /*
     * Purpose: Return a fewest-edges path between two nodes.
     * Design: Uses breadth-first search with parent links, ignoring edge weights.
     * Workflow: BFS from the source recording parents, then reconstruct the path to the target.
     * Data Handoff: Returns the node Targets along the path, or an empty vector when unreachable.
     */
    std::vector<Target> shortestPathUnweighted(const TargetGraph& graph, std::size_t from, std::size_t to)
    {
        const std::size_t nodeCount = graph.nodeCount();
        std::vector<Target> path;
        if (from >= nodeCount || to >= nodeCount)
        {
            return path;
        }

        std::vector<bool> visited(nodeCount, false);
        std::vector<std::size_t> parent(nodeCount, nodeCount);
        std::queue<std::size_t> pending;
        visited[from] = true;
        pending.push(from);

        while (!pending.empty())
        {
            const std::size_t current = pending.front();
            pending.pop();
            if (current == to)
            {
                break;
            }

            for (const GraphEdge& edge : graph.neighbors(current))
            {
                if (edge.to < nodeCount && !visited[edge.to])
                {
                    visited[edge.to] = true;
                    parent[edge.to] = current;
                    pending.push(edge.to);
                }
            }
        }

        if (!visited[to])
        {
            return path;
        }

        std::vector<std::size_t> reversed;
        for (std::size_t node = to; node != nodeCount; node = parent[node])
        {
            reversed.push_back(node);
            if (node == from)
            {
                break;
            }
        }

        for (std::size_t index = reversed.size(); index > 0; --index)
        {
            path.push_back(graph.node(reversed[index - 1]));
        }
        return path;
    }
}
