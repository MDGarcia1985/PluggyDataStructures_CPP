/*
 * File: FrequencyAnalysis.cpp
 * Description: Implements frequency ranking and bucket analysis over TargetMap entries.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/maps/FrequencyAnalysis.h"

#include <algorithm>


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        /*
         * Purpose: Copy map entries into a vector for frequency-based sorting.
         * Design: Keeps TargetMap storage read-only while algorithms own ordering.
         * Workflow: Reserve space and copy each key/count pair from the map snapshot.
         * Data Handoff: Supplies ranking helpers with a mutable working collection.
         */
        std::vector<std::pair<std::string, int>> entriesAsVector(const TargetMap& map)
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

    /*
     * Purpose: Return the N highest-frequency words with alphabetical tie-breaking.
     * Design: Sorts by descending count, then ascending key, and truncates to N.
     * Workflow: Copy entries, sort by frequency, and return the first N pairs.
     * Data Handoff: Reads TargetMap frequencies and returns a ranked snapshot.
     */
    std::vector<std::pair<std::string, int>> topN(const TargetMap& map, std::size_t n)
    {
        std::vector<std::pair<std::string, int>> entries = entriesAsVector(map);
        std::sort(entries.begin(), entries.end(),
            [](const std::pair<std::string, int>& left, const std::pair<std::string, int>& right)
            {
                if (left.second != right.second)
                {
                    return left.second > right.second;
                }
                return left.first < right.first;
            });

        if (entries.size() > n)
        {
            entries.resize(n);
        }
        return entries;
    }

    /*
     * Purpose: Return the N lowest-frequency words with alphabetical tie-breaking.
     * Design: Sorts by ascending count, then ascending key, and truncates to N.
     * Workflow: Copy entries, sort by ascending frequency, and return the first N pairs.
     * Data Handoff: Reads TargetMap frequencies and returns a ranked snapshot.
     */
    std::vector<std::pair<std::string, int>> leastFrequent(const TargetMap& map, std::size_t n)
    {
        std::vector<std::pair<std::string, int>> entries = entriesAsVector(map);
        std::sort(entries.begin(), entries.end(),
            [](const std::pair<std::string, int>& left, const std::pair<std::string, int>& right)
            {
                if (left.second != right.second)
                {
                    return left.second < right.second;
                }
                return left.first < right.first;
            });

        if (entries.size() > n)
        {
            entries.resize(n);
        }
        return entries;
    }

    /*
     * Purpose: Group words by their shared frequency count.
     * Design: Builds a frequency-to-words map with alphabetically ordered word lists.
     * Workflow: Iterate entries, append each word to its count bucket, and return the buckets.
     * Data Handoff: Reads TargetMap frequencies and returns grouped analysis data.
     */
    std::map<int, std::vector<std::string>> frequencyBuckets(const TargetMap& map)
    {
        std::map<int, std::vector<std::string>> buckets;
        for (const auto& entry : map.frequencies())
        {
            buckets[entry.second].push_back(entry.first);
        }
        return buckets;
    }
}
