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
        /*
         * Purpose: Create an unvisited marker for every graph node id.
         * Design: Sizes a bit vector once for constant-time indexed access.
         * Workflow: Allocate nodeCount flags initialized to false.
         * Data Handoff: Receives graph capacity and creates traversal-local state.
         */
        explicit VisitedSet(std::size_t nodeCount)
            : visited_(nodeCount, false)
        {
        }

        /*
         * Purpose: Report whether a valid node id has already been marked.
         * Design: Combines bounds protection with constant-time indexed lookup.
         * Workflow: Validate the id and return its stored bit when valid.
         * Data Handoff: Receives a node id and returns its visited state.
         */
        bool contains(std::size_t id) const
        {
            return id < visited_.size() && visited_[id];
        }

        /*
         * Purpose: Mark a valid node id as visited.
         * Design: Silently ignores out-of-range ids to protect traversal state.
         * Workflow: Check bounds and set the corresponding bit.
         * Data Handoff: Receives a node id and mutates its marker when valid.
         */
        void add(std::size_t id)
        {
            if (id < visited_.size())
            {
                visited_[id] = true;
            }
        }

        /*
         * Purpose: Report how many node ids can be represented.
         * Design: Uses the bit-vector size as the authoritative capacity.
         * Workflow: Return visited_.size().
         * Data Handoff: Gives traversal code the marker set's fixed range.
         */
        std::size_t capacity() const
        {
            return visited_.size();
        }
    };
}
