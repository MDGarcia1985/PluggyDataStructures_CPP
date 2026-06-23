/*
 * File: KeyNormalization.cpp
 * Description: Implements case-insensitive key normalization shared by keyed structures.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/common/KeyNormalization.h"

#include <algorithm>
#include <cctype>


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Produce a lowercase copy of a key for case-insensitive comparison.
     * Design: Works on a copy so stored Target values are never altered.
     * Workflow: Lowercase each byte with a safe unsigned cast and return the converted text.
     * Data Handoff: Supplies trees, graphs, hashing, and maps with comparable key text.
     */
    std::string normalizeKey(std::string text)
    {
        std::transform(text.begin(), text.end(), text.begin(),
            [](unsigned char character)
            {
                return static_cast<char>(std::tolower(character));
            });
        return text;
    }
}
