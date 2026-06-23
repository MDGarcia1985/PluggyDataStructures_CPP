/*
 * File: TraversalEngine.h
 * Description: Declares generic, UI-free breadth-first and depth-first id traversals.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>
#include <queue>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    using NodeId = std::size_t;

    /*
     * Purpose: Visit reachable node ids in breadth-first order from a start id.
     * Design: Stays generic by reading neighbors through a caller-supplied provider; never touches a concrete structure or UI.
     * Workflow: Mark and enqueue the start, then dequeue ids and enqueue unvisited neighbors in provider order.
     * Data Handoff: Receives a neighbor provider and returns the visited ids in discovery order.
     */
    template <typename NeighborProvider>
    std::vector<NodeId> breadthFirstIds(
        NodeId start,
        std::size_t nodeCount,
        NeighborProvider neighbors)
    {
        std::vector<NodeId> order;
        if (start >= nodeCount)
        {
            return order;
        }

        std::vector<bool> visited(nodeCount, false);
        std::queue<NodeId> pending;
        visited[start] = true;
        pending.push(start);

        while (!pending.empty())
        {
            const NodeId current = pending.front();
            pending.pop();
            order.push_back(current);

            for (const NodeId next : neighbors(current))
            {
                if (next < nodeCount && !visited[next])
                {
                    visited[next] = true;
                    pending.push(next);
                }
            }
        }

        return order;
    }

    /*
     * Purpose: Visit reachable node ids in depth-first order from a start id.
     * Design: Uses an explicit stack and pushes neighbors in reverse so provider order is preserved on visit.
     * Workflow: Push the start, then pop ids, recording first visits and stacking unvisited neighbors in reverse.
     * Data Handoff: Receives a neighbor provider and returns the visited ids in discovery order.
     */
    template <typename NeighborProvider>
    std::vector<NodeId> depthFirstIds(
        NodeId start,
        std::size_t nodeCount,
        NeighborProvider neighbors)
    {
        std::vector<NodeId> order;
        if (start >= nodeCount)
        {
            return order;
        }

        std::vector<bool> visited(nodeCount, false);
        std::vector<NodeId> pending;
        pending.push_back(start);

        while (!pending.empty())
        {
            const NodeId current = pending.back();
            pending.pop_back();

            if (current >= nodeCount || visited[current])
            {
                continue;
            }

            visited[current] = true;
            order.push_back(current);

            std::vector<NodeId> adjacent;
            for (const NodeId next : neighbors(current))
            {
                adjacent.push_back(next);
            }
            for (std::size_t index = adjacent.size(); index > 0; --index)
            {
                const NodeId next = adjacent[index - 1];
                if (next < nodeCount && !visited[next])
                {
                    pending.push_back(next);
                }
            }
        }

        return order;
    }
}
