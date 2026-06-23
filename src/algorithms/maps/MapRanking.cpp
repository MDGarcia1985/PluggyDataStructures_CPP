/*
 * File: MapRanking.cpp
 * Description: Implements ranking views over TargetMap word frequencies.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/maps/MapRanking.h"

#include <algorithm>


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Return every word/count pair sorted by descending frequency.
     * Design: Uses alphabetical order as a stable tie-breaker for equal counts.
     * Workflow: Copy entries, sort by frequency, and return the ranked vector.
     * Data Handoff: Reads TargetMap frequencies and returns a sorted snapshot.
     */
    std::vector<std::pair<std::string, int>> sortByFrequency(const TargetMap& map)
    {
        std::vector<std::pair<std::string, int>> entries;
        entries.reserve(map.frequencies().size());
        for (const auto& entry : map.frequencies())
        {
            entries.emplace_back(entry.first, entry.second);
        }

        std::sort(entries.begin(), entries.end(),
            [](const std::pair<std::string, int>& left, const std::pair<std::string, int>& right)
            {
                if (left.second != right.second)
                {
                    return left.second > right.second;
                }
                return left.first < right.first;
            });
        return entries;
    }

    /*
     * Purpose: Return every word/count pair sorted alphabetically by word.
     * Design: Mirrors std::map key order while materializing a vector snapshot.
     * Workflow: Copy entries in map order and return the vector.
     * Data Handoff: Reads TargetMap frequencies and returns an alphabetical snapshot.
     */
    std::vector<std::pair<std::string, int>> sortAlphabetically(const TargetMap& map)
    {
        std::vector<std::pair<std::string, int>> entries;
        entries.reserve(map.frequencies().size());
        for (const auto& entry : map.frequencies())
        {
            entries.emplace_back(entry.first, entry.second);
        }
        return entries;
    }
}
