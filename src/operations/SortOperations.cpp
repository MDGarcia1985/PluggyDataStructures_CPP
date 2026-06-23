/*
 * File: SortOperations.cpp
 * Description: Registers sort menu commands that adapt the active list to pure sorting algorithms.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/sorting/InsertionSort.h"
#include "algorithms/sorting/QuickSort.h"
#include "algorithms/sorting/SelectionSort.h"
#include "algorithms/sorting/SortBenchmark.h"
#include "core/TargetProgram.h"
#include "registry/SortRegistry.h"

#include <iomanip>
#include <iostream>


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        /*
         * Purpose: Snapshot the active list, benchmark a pure sort, replace the list, and report timing.
         * Design: This adapter is the only sorting code that knows about TargetProgram, list replacement, and output.
         * Workflow: Snapshot to a vector, guard the empty case, benchmark, rebuild the list, then print metrics.
         * Data Handoff: Moves the sorted benchmark result back into the program's TargetList.
         */
        void runRegisteredSort(TargetProgram& program, const std::string& algorithmName, SortFunction sortFunction)
        {
            const std::vector<Target> baseline = program.list().toVector();
            if (baseline.empty())
            {
                std::cout << "\nThe target list is empty. Nothing was sorted.\n";
                return;
            }

            const SortBenchmarkResult result = benchmarkSort(algorithmName, baseline, std::move(sortFunction));

            program.list().clear();
            for (const Target& target : result.sortedTargets)
            {
                program.list().addBack(target);
            }

            std::cout << std::fixed << std::setprecision(9);
            std::cout << "\nSort algorithm: " << result.algorithmName << "\n";
            std::cout << "Array size: " << result.itemCount << "\n";
            std::cout << "Sort repetitions: " << result.repetitions << "\n";
            std::cout << "Total sort time: " << result.totalSeconds << " seconds\n";
            std::cout << "Average sort time: " << result.averageSeconds << " seconds\n";
            std::cout << "Linked list replaced with the sorted records.\n";
        }
    }
}

PDS_REGISTER_SORT("Insertion Sort",
    [](pds::TargetProgram& program) { pds::runRegisteredSort(program, "Insertion Sort", pds::insertionSort); })

PDS_REGISTER_SORT("Selection Sort",
    [](pds::TargetProgram& program) { pds::runRegisteredSort(program, "Selection Sort", pds::selectionSort); })

PDS_REGISTER_SORT("Quick Sort",
    [](pds::TargetProgram& program) { pds::runRegisteredSort(program, "Quick Sort", pds::quickSort); })
