/*
 * File: GraphAnalysis.cpp
 * Description: Implements degree, undirected connected components, and directed cycle detection.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/graphs/GraphAnalysis.h"

#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        /*
         * Purpose: Build a symmetric neighbor table so components ignore edge direction.
         * Design: Adds each stored edge in both directions to model undirected connectivity.
         * Workflow: Walk every node's adjacency and record both endpoints as mutual neighbors.
         * Data Handoff: Supplies the component scan with an undirected adjacency view.
         */
        std::vector<std::vector<std::size_t>> symmetricAdjacency(const TargetGraph& graph)
        {
            const std::size_t nodeCount = graph.nodeCount();
            std::vector<std::vector<std::size_t>> undirected(nodeCount);

            for (std::size_t from = 0; from < nodeCount; ++from)
            {
                for (const GraphEdge& edge : graph.neighbors(from))
                {
                    if (edge.to < nodeCount)
                    {
                        undirected[from].push_back(edge.to);
                        undirected[edge.to].push_back(from);
                    }
                }
            }

            return undirected;
        }

        bool visitForCycle(
            const TargetGraph& graph,
            std::size_t node,
            std::size_t parent,
            std::vector<int>& state)
        {
            state[node] = 1;

            for (const GraphEdge& edge : graph.neighbors(node))
            {
                if (edge.to >= graph.nodeCount())
                {
                    continue;
                }
                if (!edge.directed && edge.to == parent)
                {
                    continue;
                }
                if (state[edge.to] == 1)
                {
                    return true;
                }
                if (state[edge.to] == 0 && visitForCycle(graph, edge.to, node, state))
                {
                    return true;
                }
            }

            state[node] = 2;
            return false;
        }
    }

    /*
     * Purpose: Report the out-degree of a node.
     * Design: Counts the stored outgoing edges for the id.
     * Workflow: Return the neighbor row size, or zero for an invalid id.
     * Data Handoff: Supplies the session with a per-node connectivity metric.
     */
    std::size_t degree(const TargetGraph& graph, std::size_t id)
    {
        if (id >= graph.nodeCount())
        {
            return 0;
        }
        return graph.neighbors(id).size();
    }

    /*
     * Purpose: Count connected components treating edges as undirected.
     * Design: Builds a symmetric adjacency, then counts components with an iterative flood fill.
     * Workflow: For each unvisited node, expand all reachable nodes and increment the component count.
     * Data Handoff: Returns the number of undirected components in the graph.
     */
    std::size_t connectedComponents(const TargetGraph& graph)
    {
        const std::size_t nodeCount = graph.nodeCount();
        if (nodeCount == 0)
        {
            return 0;
        }

        const std::vector<std::vector<std::size_t>> undirected = symmetricAdjacency(graph);
        std::vector<bool> visited(nodeCount, false);
        std::size_t components = 0;

        for (std::size_t start = 0; start < nodeCount; ++start)
        {
            if (visited[start])
            {
                continue;
            }

            ++components;
            std::vector<std::size_t> pending;
            pending.push_back(start);
            visited[start] = true;

            while (!pending.empty())
            {
                const std::size_t current = pending.back();
                pending.pop_back();
                for (const std::size_t next : undirected[current])
                {
                    if (!visited[next])
                    {
                        visited[next] = true;
                        pending.push_back(next);
                    }
                }
            }
        }

        return components;
    }

    /*
     * Purpose: Report whether the directed graph contains a cycle.
     * Design: Uses a three-color DFS where a back edge to a node on the stack signals a cycle.
     * Workflow: Run a colored DFS from each unvisited node, returning true on the first back edge.
     * Data Handoff: Returns a single boolean cycle verdict.
     */
    bool hasCycle(const TargetGraph& graph)
    {
        const std::size_t nodeCount = graph.nodeCount();
        std::vector<int> state(nodeCount, 0);

        for (std::size_t node = 0; node < nodeCount; ++node)
        {
            if (state[node] == 0 && visitForCycle(graph, node, nodeCount, state))
            {
                return true;
            }
        }

        return false;
    }
}
