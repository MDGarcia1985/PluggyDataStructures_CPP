/*
 * File: MapSearch.h
 * Description: Declares lookup helpers over TargetMap word frequencies.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "structures/TargetMap.h"

#include <string>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    bool containsWord(const TargetMap& map, const std::string& word);
    std::vector<std::string> wordsStartingWith(const TargetMap& map, const std::string& prefix);
}
