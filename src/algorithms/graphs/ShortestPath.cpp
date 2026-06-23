/*
 * File: ShortestPath.cpp
 * Description: Implements a minimal Dijkstra shortest path over non-negative edge weights.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/graphs/ShortestPath.h"

#include <limits>
#include <queue>
#include <utility>


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Find the least-weight path between two nodes using Dijkstra's algorithm.
     * Design: Minimal implementation assuming non-negative weights; uses a lazy min-heap of (distance, node).
     * Workflow: Relax neighbors from the closest unsettled node, then reconstruct the path via parents.
     * Data Handoff: Returns reachability, total distance, and the node path back to the caller.
     */
    ShortestPathResult shortestPath(const TargetGraph& graph, std::size_t from, std::size_t to)
    {
        ShortestPathResult result;
        const std::size_t nodeCount = graph.nodeCount();
        if (from >= nodeCount || to >= nodeCount)
        {
            return result;
        }

        const double infinity = std::numeric_limits<double>::infinity();
        std::vector<double> distance(nodeCount, infinity);
        std::vector<std::size_t> parent(nodeCount, nodeCount);

        using Entry = std::pair<double, std::size_t>;
        std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> frontier;
        distance[from] = 0.0;
        frontier.push(Entry{0.0, from});

        while (!frontier.empty())
        {
            const Entry top = frontier.top();
            frontier.pop();
            const double currentDistance = top.first;
            const std::size_t current = top.second;

            if (currentDistance > distance[current])
            {
                continue;
            }
            if (current == to)
            {
                break;
            }

            for (const GraphEdge& edge : graph.neighbors(current))
            {
                if (edge.to >= nodeCount)
                {
                    continue;
                }
                const double candidate = currentDistance + edge.weight;
                if (candidate < distance[edge.to])
                {
                    distance[edge.to] = candidate;
                    parent[edge.to] = current;
                    frontier.push(Entry{candidate, edge.to});
                }
            }
        }

        if (distance[to] == infinity)
        {
            return result;
        }

        result.reachable = true;
        result.distance = distance[to];

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
            result.path.push_back(graph.node(reversed[index - 1]));
        }

        return result;
    }
}
