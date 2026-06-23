/*
 * File: SeparateChaining.h
 * Description: Declares a separate-chaining hash strategy with collision and bucket views.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Target.h"

#include <cstddef>
#include <string>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    struct HashBucketView
    {
        std::size_t index;
        std::vector<Target> entries;
    };

    /*
     * Purpose: Store Target records in per-bucket chains keyed by the first field.
     * Design: Pure data-structure strategy with no console, menu, or registry dependency.
     * Workflow: Hash a key to a bucket vector, then insert/lookup/remove within that short chain.
     * Data Handoff: Receives Targets and keys from a hash-table facade and reports chain metrics.
     */
    class SeparateChaining
    {
    private:
        std::vector<std::vector<Target>> buckets_;
        std::size_t count_;

    public:
        explicit SeparateChaining(std::size_t initialBucketCount = 8);

        void insert(const Target& target);
        bool lookup(const std::string& key, Target& found) const;
        bool remove(const std::string& key);
        void rehash(std::size_t newBucketCount);
        void clear();

        bool isEmpty() const;
        std::size_t size() const;
        std::size_t bucketCount() const;
        double loadFactor() const;
        std::size_t collisionCount() const;
        std::size_t longestChain() const;

        std::vector<Target> entries() const;
        std::vector<HashBucketView> bucketViews() const;
    };
}
