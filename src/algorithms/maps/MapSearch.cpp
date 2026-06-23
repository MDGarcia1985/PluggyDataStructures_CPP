/*
 * File: MapSearch.cpp
 * Description: Implements lookup helpers over TargetMap word frequencies.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/maps/MapSearch.h"

#include <cctype>


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        /*
         * Purpose: Normalize a search token to match stored map keys.
         * Design: Retains only alphanumeric bytes and lowercases them like TargetMap storage.
         * Workflow: Scan the token, keep alphanumeric characters, and lowercase each byte.
         * Data Handoff: Returns a canonical key for map lookup and prefix search.
         */
        std::string normalizeSearchWord(const std::string& word)
        {
            std::string normalized;
            normalized.reserve(word.size());

            for (const char character : word)
            {
                const unsigned char value = static_cast<unsigned char>(character);
                if (std::isalnum(value))
                {
                    normalized.push_back(static_cast<char>(std::tolower(value)));
                }
            }

            return normalized;
        }
    }

    /*
     * Purpose: Report whether a normalized word exists in the map.
     * Design: Normalizes the query before checking the ordered frequency map.
     * Workflow: Canonicalize the word and test for membership in frequencies().
     * Data Handoff: Reads TargetMap storage and returns a boolean lookup result.
     */
    bool containsWord(const TargetMap& map, const std::string& word)
    {
        const std::string normalized = normalizeSearchWord(word);
        return !normalized.empty() && map.frequencies().count(normalized) > 0;
    }

    /*
     * Purpose: Return every stored word whose normalized key starts with a prefix.
     * Design: Uses alphabetical map iteration so results remain sorted.
     * Workflow: Normalize the prefix, scan keys, and collect matching words.
     * Data Handoff: Reads TargetMap frequencies and returns matching keys.
     */
    std::vector<std::string> wordsStartingWith(const TargetMap& map, const std::string& prefix)
    {
        const std::string normalizedPrefix = normalizeSearchWord(prefix);
        std::vector<std::string> matches;

        if (normalizedPrefix.empty())
        {
            return matches;
        }

        for (const auto& entry : map.frequencies())
        {
            if (entry.first.compare(0, normalizedPrefix.size(), normalizedPrefix) == 0)
            {
                matches.push_back(entry.first);
            }
        }

        return matches;
    }
}
