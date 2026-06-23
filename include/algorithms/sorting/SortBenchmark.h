/*
 * File: SortBenchmark.h
 * Description: Declares timing, repetition policy, and result types for sorting benchmarks.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Target.h"

#include <cstddef>
#include <functional>
#include <string>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    using SortFunction = std::function<void(std::vector<Target>&)>;

    struct SortBenchmarkResult
    {
        std::string algorithmName;
        std::size_t itemCount = 0;
        std::size_t repetitions = 0;
        double totalSeconds = 0.0;
        double averageSeconds = 0.0;
        std::vector<Target> sortedTargets;
    };

    std::size_t repetitionsForSize(std::size_t size);

    SortBenchmarkResult benchmarkSort(
        const std::string& algorithmName,
        const std::vector<Target>& baseline,
        SortFunction sortFunction);
}
