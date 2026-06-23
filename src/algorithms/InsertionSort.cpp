/*
 * File: InsertionSort.cpp
 * Description: Implements and registers insertion sort.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "registry/SortRegistry.h"
#include "sorting/SortSupport.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        /*
         * Purpose: Sort a Target array with insertion sort.
         * Design: Receives a prepared vector from SortSupport.
         * Workflow: Shift larger items right, then insert the saved item into place.
         * Data Handoff: Mutates the prepared vector supplied by the shared timing workflow.
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

        /*
         * Purpose: Connect insertion sort to the shared sort execution workflow.
         * Design: Keeps registration glue local while SortSupport owns preparation and reporting.
         * Workflow: Receive the active program and invoke runSortCommand() with insertionSort.
         * Data Handoff: Passes the program and algorithm function into shared sorting support.
         */
        void insertionSortCommand(TargetProgram& program)
        {
            runSortCommand(program, "Insertion Sort", insertionSort);
        }
    }
}

PDS_REGISTER_SORT("Insertion Sort", pds::insertionSortCommand)
