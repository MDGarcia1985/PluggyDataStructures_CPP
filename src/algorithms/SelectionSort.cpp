/*
 * File: SelectionSort.cpp
 * Description: Implements and registers selection sort.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "registry/SortRegistry.h"
#include "sorting/SortSupport.h"


// named container llb = linked list browser
namespace llb
{
    namespace
    {
        /*
         * Purpose: Sort a Target array with selection sort.
         * Design: Receives a prepared vector from SortSupport.
         * Workflow: Find the smallest remaining item and swap it into the current position.
         * Data Handoff: Mutates the prepared vector supplied by the shared timing workflow.
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

        /*
         * Purpose: Connect selection sort to the shared sort execution workflow.
         * Design: Keeps registration glue local while SortSupport owns preparation and reporting.
         * Workflow: Receive the active program and invoke runSortCommand() with selectionSort.
         * Data Handoff: Passes the program and algorithm function into shared sorting support.
         */
        void selectionSortCommand(TargetProgram& program)
        {
            runSortCommand(program, "Selection Sort", selectionSort);
        }
    }
}

LLB_REGISTER_SORT("Selection Sort", llb::selectionSortCommand)
