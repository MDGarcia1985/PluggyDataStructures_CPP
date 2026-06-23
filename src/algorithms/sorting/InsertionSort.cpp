/*
 * File: InsertionSort.cpp
 * Description: Implements pure insertion sort using the shared Target ordering.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/sorting/InsertionSort.h"

#include "algorithms/common/TargetOrdering.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Sort a Target vector in ascending order with insertion sort.
     * Design: Pure function with no I/O, timing, or list ownership; comparison comes from TargetOrdering.
     * Workflow: Shift larger items right, then insert the saved item into its sorted position.
     * Data Handoff: Mutates the caller-owned vector in place.
     */
    void insertionSort(std::vector<Target>& targets)
    {
        for (std::size_t i = 1; i < targets.size(); ++i)
        {
            Target current = targets[i];
            std::size_t j = i;

            while (j > 0 && targetLess(current, targets[j - 1]))
            {
                targets[j] = targets[j - 1];
                --j;
            }

            targets[j] = current;
        }
    }
}
