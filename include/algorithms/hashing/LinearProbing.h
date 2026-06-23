/*
 * File: LinearProbing.h
 * Description: Declares an open-addressing hash strategy with tombstones and probe metrics.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "algorithms/hashing/SeparateChaining.h"
#include "core/Target.h"

#include <cstddef>
#include <string>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    enum class ProbeSlotState
    {
        Empty,
        Occupied,
        Tombstone
    };

    /*
     * Purpose: Store Target records in a single open-addressed array using linear probing.
     * Design: Pure data-structure strategy with tombstone deletion and no console/menu/registry ties.
     * Workflow: Probe forward from the home slot for insert/lookup/remove, resizing on high load.
     * Data Handoff: Receives Targets and keys from a hash-table facade and reports probe metrics.
     */
    class LinearProbing
    {
    private:
        std::vector<Target> slots_;
        std::vector<ProbeSlotState> states_;
        std::size_t count_;

    public:
        explicit LinearProbing(std::size_t initialCapacity = 8);

        void insert(const Target& target);
        bool lookup(const std::string& key, Target& found) const;
        bool remove(const std::string& key);
        void rehash(std::size_t newCapacity);
        void clear();

        bool isEmpty() const;
        std::size_t size() const;
        std::size_t capacity() const;
        double loadFactor() const;
        std::size_t probeCountFor(const std::string& key) const;
        std::size_t totalProbeCount() const;
        std::size_t longestCluster() const;

        std::vector<Target> entries() const;
        std::vector<HashBucketView> slotViews() const;
    };
}
