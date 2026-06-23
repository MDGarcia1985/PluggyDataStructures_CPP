/*
 * File: SelectionSort.cpp
 * Description: Implements pure selection sort using the shared Target ordering.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/sorting/SelectionSort.h"

#include "algorithms/common/TargetOrdering.h"

#include <utility>


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Sort a Target vector in ascending order with selection sort.
     * Design: Pure function with no I/O, timing, or list ownership; comparison comes from TargetOrdering.
     * Workflow: Find the smallest remaining item and swap it into the current position.
     * Data Handoff: Mutates the caller-owned vector in place.
     */
    void selectionSort(std::vector<Target>& targets)
    {
        for (std::size_t i = 0; i < targets.size(); ++i)
        {
            std::size_t minimumIndex = i;

            for (std::size_t j = i + 1; j < targets.size(); ++j)
            {
                if (targetLess(targets[j], targets[minimumIndex]))
                {
                    minimumIndex = j;
                }
            }

            if (minimumIndex != i)
            {
                std::swap(targets[i], targets[minimumIndex]);
            }
        }
    }
}
