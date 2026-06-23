/*
 * File: TargetHashTable.h
 * Description: Declares a hash table facade delegating to separate-chaining or linear-probing strategies.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "algorithms/hashing/LinearProbing.h"
#include "algorithms/hashing/SeparateChaining.h"
#include "core/Target.h"

#include <cstddef>
#include <string>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    enum class HashStrategy
    {
        SeparateChaining,
        LinearProbing
    };

    /*
     * Purpose: Present one hash-table API while delegating storage to a selected collision strategy.
     * Design: Owns both strategy objects but routes operations to the active one, keeping algorithm detail in the strategies.
     * Workflow: Forward insert/find/erase to the active strategy and migrate entries when the strategy changes.
     * Data Handoff: Receives Targets and keys from HashTableSession and serves views and metrics back.
     */
    class TargetHashTable
    {
    private:
        HashStrategy strategy_;
        SeparateChaining chaining_;
        LinearProbing probing_;

    public:
        explicit TargetHashTable(std::size_t initialBuckets = 8);

        void insert(const Target& target);
        bool find(const std::string& key, Target& found) const;
        bool erase(const std::string& key);

        bool isEmpty() const;
        std::size_t size() const;
        std::size_t bucketCount() const;
        double loadFactor() const;
        std::vector<HashBucketView> bucketViews() const;

        HashStrategy strategy() const;
        std::string strategyName() const;
        void useStrategy(HashStrategy strategy);

        std::size_t collisionCount() const;
        std::size_t longestRun() const;
        double averageProbeLength() const;
    };
}
