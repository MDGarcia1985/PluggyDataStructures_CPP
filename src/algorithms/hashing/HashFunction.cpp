/*
 * File: HashFunction.cpp
 * Description: Implements case-insensitive string hashing and bucket-index mapping.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/hashing/HashFunction.h"

#include "algorithms/common/KeyNormalization.h"

#include <functional>


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Hash a key independent of letter case.
     * Design: Normalizes the key first so equal keys differing only in case hash identically.
     * Workflow: Normalize the key and apply the standard string hash.
     * Data Handoff: Supplies bucket-index mapping and collision tooling with a stable hash value.
     */
    std::size_t hashKey(const std::string& key)
    {
        return std::hash<std::string>{}(normalizeKey(key));
    }

    /*
     * Purpose: Map a key to a slot index within a given table size.
     * Design: Reduces the normalized hash modulo the table size, treating zero size as one.
     * Workflow: Hash the key and take the remainder over the bucket count.
     * Data Handoff: Supplies separate chaining and linear probing with the home index for a key.
     */
    std::size_t bucketIndexFor(const std::string& key, std::size_t bucketCount)
    {
        const std::size_t divisor = bucketCount < 1 ? 1 : bucketCount;
        return hashKey(key) % divisor;
    }
}
