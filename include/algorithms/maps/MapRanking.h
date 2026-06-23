/*
 * File: MapRanking.h
 * Description: Declares ranking views over TargetMap word frequencies.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "structures/TargetMap.h"

#include <string>
#include <utility>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    std::vector<std::pair<std::string, int>> sortByFrequency(const TargetMap& map);
    std::vector<std::pair<std::string, int>> sortAlphabetically(const TargetMap& map);
}
