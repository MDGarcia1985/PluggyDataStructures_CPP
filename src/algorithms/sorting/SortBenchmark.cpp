/*
 * File: SortBenchmark.cpp
 * Description: Implements repetition policy and timing for pure sorting functions.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/sorting/SortBenchmark.h"

#include <chrono>
#include <utility>


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Choose a practical timing repetition count from the dataset size.
     * Design: Uses fewer repetitions for larger quadratic sorting workloads.
     * Workflow: Compare the size against thresholds and return the matching count.
     * Data Handoff: Supplies benchmarkSort() with a benchmark loop count.
     */
    std::size_t repetitionsForSize(std::size_t size)
    {
        if (size < 100)
        {
            return 1000;
        }
        if (size < 1000)
        {
            return 100;
        }
        if (size < 5000)
        {
            return 20;
        }
        return 3;
    }

    /*
     * Purpose: Time a pure sorting function over repeated copies of a baseline dataset.
     * Design: Owns timing and repetition policy only; never touches a TargetList, program, or console.
     * Workflow: Clone the baseline per repetition, time the sort loop, average, and keep one sorted copy.
     * Data Handoff: Returns the sorted result plus timing metadata for the calling glue to apply and report.
     */
    SortBenchmarkResult benchmarkSort(
        const std::string& algorithmName,
        const std::vector<Target>& baseline,
        SortFunction sortFunction)
    {
        SortBenchmarkResult result;
        result.algorithmName = algorithmName;
        result.itemCount = baseline.size();
        result.repetitions = repetitionsForSize(baseline.size());

        std::vector<std::vector<Target>> timedArrays(result.repetitions, baseline);

        const auto startTime = std::chrono::steady_clock::now();
        for (std::vector<Target>& timedArray : timedArrays)
        {
            sortFunction(timedArray);
        }
        const auto endTime = std::chrono::steady_clock::now();

        const std::chrono::duration<double> elapsedSeconds = endTime - startTime;
        result.totalSeconds = elapsedSeconds.count();
        result.averageSeconds = result.repetitions > 0
            ? result.totalSeconds / static_cast<double>(result.repetitions)
            : 0.0;

        if (!timedArrays.empty())
        {
            result.sortedTargets = std::move(timedArrays.front());
        }
        else
        {
            result.sortedTargets = baseline;
            sortFunction(result.sortedTargets);
        }

        return result;
    }
}
