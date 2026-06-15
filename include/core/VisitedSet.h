/*
 * File: VisitedSet.h
 * Description: Declares a constant-time visited marker for integer-id graph traversal.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Track which integer-id graph nodes have already been visited.
     * Design: Backs the set with a bit vector indexed by node id for O(1) test and mark.
     * Workflow: Construct sized to the node count, then add and query node ids during traversal.
     * Data Handoff: Receives node ids from BFS or DFS and reports whether each was already seen.
     */
    class VisitedSet
    {
    private:
        std::vector<bool> visited_;

    public:
        explicit VisitedSet(std::size_t nodeCount)
            : visited_(nodeCount, false)
        {
        }

        bool contains(std::size_t id) const
        {
            return id < visited_.size() && visited_[id];
        }

        void add(std::size_t id)
        {
            if (id < visited_.size())
            {
                visited_[id] = true;
            }
        }

        std::size_t capacity() const
        {
            return visited_.size();
        }
    };
}
