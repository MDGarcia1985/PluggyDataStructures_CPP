/*
 * File: FrequencyAnalysis.h
 * Description: Declares frequency ranking and bucket analysis over TargetMap entries.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "structures/TargetMap.h"

#include <cstddef>
#include <map>
#include <string>
#include <utility>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    std::vector<std::pair<std::string, int>> topN(const TargetMap& map, std::size_t n);
    std::vector<std::pair<std::string, int>> leastFrequent(const TargetMap& map, std::size_t n);
    std::map<int, std::vector<std::string>> frequencyBuckets(const TargetMap& map);
}
