/*
 * File: QuickSort.cpp
 * Description: Implements pure, deterministic quick sort using the shared Target ordering.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/sorting/QuickSort.h"

#include "algorithms/common/TargetOrdering.h"

#include <utility>


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        /*
         * Purpose: Partition a sub-range around the last element as pivot (Lomuto scheme).
         * Design: Deterministic pivot choice keeps results reproducible for teaching and tests.
         * Workflow: Move items less than the pivot left, then place the pivot after them.
         * Data Handoff: Receives range bounds and returns the pivot's final index.
         */
        std::size_t partitionRange(std::vector<Target>& targets, std::size_t low, std::size_t high)
        {
            const Target pivot = targets[high];
            std::size_t boundary = low;

            for (std::size_t scan = low; scan < high; ++scan)
            {
                if (targetLess(targets[scan], pivot))
                {
                    std::swap(targets[boundary], targets[scan]);
                    ++boundary;
                }
            }

            std::swap(targets[boundary], targets[high]);
            return boundary;
        }

        /*
         * Purpose: Recursively sort an inclusive index range.
         * Design: Uses size_t-safe bounds so empty and single-element ranges terminate immediately.
         * Workflow: Partition the range, then sort the left and right partitions.
         * Data Handoff: Mutates the shared vector in place between the given bounds.
         */
        void quickSortRange(std::vector<Target>& targets, std::size_t low, std::size_t high)
        {
            if (low >= high || high >= targets.size())
            {
                return;
            }

            const std::size_t pivotIndex = partitionRange(targets, low, high);
            if (pivotIndex > low)
            {
                quickSortRange(targets, low, pivotIndex - 1);
            }
            quickSortRange(targets, pivotIndex + 1, high);
        }
    }

    /*
     * Purpose: Sort a Target vector in ascending order with quick sort.
     * Design: Pure function with no I/O, timing, or list ownership; comparison comes from TargetOrdering.
     * Workflow: Guard empty/single inputs, then recursively partition the full range.
     * Data Handoff: Mutates the caller-owned vector in place.
     */
    void quickSort(std::vector<Target>& targets)
    {
        if (targets.size() < 2)
        {
            return;
        }
        quickSortRange(targets, 0, targets.size() - 1);
    }
}
