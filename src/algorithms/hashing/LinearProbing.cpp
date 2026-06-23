/*
 * File: LinearProbing.cpp
 * Description: Implements open-addressing insert, lookup, tombstone removal, resize, and metrics.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/hashing/LinearProbing.h"

#include "algorithms/common/KeyNormalization.h"
#include "algorithms/hashing/HashFunction.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        constexpr double MAX_LOAD_FACTOR = 0.5;
    }

    /*
     * Purpose: Create an open-addressed table with a starting capacity.
     * Design: Allocates at least one slot so the modulo home index is always valid.
     * Workflow: Size the slot and state vectors and start the element count at zero.
     * Data Handoff: Produces a ready-to-fill strategy for the hash-table facade or tests.
     */
    LinearProbing::LinearProbing(std::size_t initialCapacity)
        : slots_(initialCapacity < 1 ? 1 : initialCapacity),
          states_(initialCapacity < 1 ? 1 : initialCapacity, ProbeSlotState::Empty),
          count_(0)
    {
    }

    /*
     * Purpose: Insert a Target, or update the value stored under an existing key.
     * Design: Probes forward from the home slot, reusing the first tombstone seen, and resizes on high load.
     * Workflow: Scan for the key or a free slot, place the entry, then rehash when the load factor is exceeded.
     * Data Handoff: Receives a Target and stores it at a probed slot.
     */
    void LinearProbing::insert(const Target& target)
    {
        const std::string key = normalizeKey(target.fieldOne());
        const std::size_t capacity = slots_.size();
        std::size_t home = bucketIndexFor(key, capacity);

        bool tombstoneSeen = false;
        std::size_t tombstoneIndex = 0;

        for (std::size_t step = 0; step < capacity; ++step)
        {
            const std::size_t index = (home + step) % capacity;
            const ProbeSlotState state = states_[index];

            if (state == ProbeSlotState::Occupied)
            {
                if (normalizeKey(slots_[index].fieldOne()) == key)
                {
                    slots_[index] = target;
                    return;
                }
            }
            else if (state == ProbeSlotState::Tombstone)
            {
                if (!tombstoneSeen)
                {
                    tombstoneSeen = true;
                    tombstoneIndex = index;
                }
            }
            else
            {
                const std::size_t placement = tombstoneSeen ? tombstoneIndex : index;
                slots_[placement] = target;
                states_[placement] = ProbeSlotState::Occupied;
                ++count_;

                if (loadFactor() > MAX_LOAD_FACTOR)
                {
                    rehash(capacity * 2);
                }
                return;
            }
        }

        if (tombstoneSeen)
        {
            slots_[tombstoneIndex] = target;
            states_[tombstoneIndex] = ProbeSlotState::Occupied;
            ++count_;
            return;
        }

        rehash(capacity * 2);
        insert(target);
    }

    /*
     * Purpose: Find a stored Target by its first-field key.
     * Design: Probes forward from the home slot, skipping tombstones until an empty slot ends the search.
     * Workflow: Compare occupied slots to the key and stop at the first empty slot.
     * Data Handoff: Writes the matched Target into found and reports whether a match existed.
     */
    bool LinearProbing::lookup(const std::string& key, Target& found) const
    {
        const std::string normalized = normalizeKey(key);
        const std::size_t capacity = slots_.size();
        const std::size_t home = bucketIndexFor(normalized, capacity);

        for (std::size_t step = 0; step < capacity; ++step)
        {
            const std::size_t index = (home + step) % capacity;
            const ProbeSlotState state = states_[index];

            if (state == ProbeSlotState::Empty)
            {
                return false;
            }
            if (state == ProbeSlotState::Occupied &&
                normalizeKey(slots_[index].fieldOne()) == normalized)
            {
                found = slots_[index];
                return true;
            }
        }

        return false;
    }

    /*
     * Purpose: Remove a stored Target by its first-field key using a tombstone.
     * Design: Marks the slot as a tombstone so later probe chains are not broken.
     * Workflow: Probe to the matching slot, mark it as a tombstone, and decrement the count.
     * Data Handoff: Receives a key and reports whether a matching entry was removed.
     */
    bool LinearProbing::remove(const std::string& key)
    {
        const std::string normalized = normalizeKey(key);
        const std::size_t capacity = slots_.size();
        const std::size_t home = bucketIndexFor(normalized, capacity);

        for (std::size_t step = 0; step < capacity; ++step)
        {
            const std::size_t index = (home + step) % capacity;
            const ProbeSlotState state = states_[index];

            if (state == ProbeSlotState::Empty)
            {
                return false;
            }
            if (state == ProbeSlotState::Occupied &&
                normalizeKey(slots_[index].fieldOne()) == normalized)
            {
                states_[index] = ProbeSlotState::Tombstone;
                slots_[index] = Target();
                --count_;
                return true;
            }
        }

        return false;
    }

    /*
     * Purpose: Resize the table and reinsert all live entries into fresh empty slots.
     * Design: Discards tombstones during rebuild so probe chains shorten after churn.
     * Workflow: Snapshot live entries, reset slot and state vectors, and reinsert each entry.
     * Data Handoff: Preserves all live Targets while changing slot placement and clearing tombstones.
     */
    void LinearProbing::rehash(std::size_t newCapacity)
    {
        if (newCapacity < 1)
        {
            newCapacity = 1;
        }

        const std::vector<Target> live = entries();
        slots_.assign(newCapacity, Target());
        states_.assign(newCapacity, ProbeSlotState::Empty);
        count_ = 0;

        for (const Target& entry : live)
        {
            insert(entry);
        }
    }

    /*
     * Purpose: Remove all entries while keeping the current capacity.
     * Design: Resets every slot to empty and zeroes the element count.
     * Workflow: Clear slot values, mark all slots empty, and zero the count.
     * Data Handoff: Leaves an empty strategy ready for reuse.
     */
    void LinearProbing::clear()
    {
        for (std::size_t index = 0; index < slots_.size(); ++index)
        {
            slots_[index] = Target();
            states_[index] = ProbeSlotState::Empty;
        }
        count_ = 0;
    }

    bool LinearProbing::isEmpty() const
    {
        return count_ == 0;
    }

    std::size_t LinearProbing::size() const
    {
        return count_;
    }

    std::size_t LinearProbing::capacity() const
    {
        return slots_.size();
    }

    double LinearProbing::loadFactor() const
    {
        return static_cast<double>(count_) / static_cast<double>(slots_.size());
    }

    /*
     * Purpose: Report how many slots are probed to locate a key.
     * Design: Counts every slot examined until the key is found or an empty slot ends the search.
     * Workflow: Walk the probe sequence, counting steps, stopping on a match or an empty slot.
     * Data Handoff: Supplies the session and collision tooling with a per-key probe cost.
     */
    std::size_t LinearProbing::probeCountFor(const std::string& key) const
    {
        const std::string normalized = normalizeKey(key);
        const std::size_t capacity = slots_.size();
        const std::size_t home = bucketIndexFor(normalized, capacity);

        for (std::size_t step = 0; step < capacity; ++step)
        {
            const std::size_t index = (home + step) % capacity;
            const ProbeSlotState state = states_[index];

            if (state == ProbeSlotState::Empty)
            {
                return step + 1;
            }
            if (state == ProbeSlotState::Occupied &&
                normalizeKey(slots_[index].fieldOne()) == normalized)
            {
                return step + 1;
            }
        }

        return capacity;
    }

    /*
     * Purpose: Sum the probe distance of every live entry from its home slot.
     * Design: Measures clustering cost as the total displacement of occupied slots.
     * Workflow: For each occupied slot, add the forward distance from its key's home index.
     * Data Handoff: Supplies average-probe-length tooling with the aggregate probe cost.
     */
    std::size_t LinearProbing::totalProbeCount() const
    {
        const std::size_t capacity = slots_.size();
        std::size_t total = 0;

        for (std::size_t index = 0; index < capacity; ++index)
        {
            if (states_[index] == ProbeSlotState::Occupied)
            {
                const std::size_t home = bucketIndexFor(normalizeKey(slots_[index].fieldOne()), capacity);
                total += ((index + capacity - home) % capacity) + 1;
            }
        }

        return total;
    }

    /*
     * Purpose: Report the longest run of consecutive occupied slots.
     * Design: Reveals primary clustering, the main weakness of linear probing.
     * Workflow: Scan slots tracking the current and maximum occupied run length.
     * Data Handoff: Supplies the session and collision tooling with a clustering metric.
     */
    std::size_t LinearProbing::longestCluster() const
    {
        const std::size_t capacity = states_.size();
        if (capacity == 0 || count_ == 0)
        {
            return 0;
        }
        if (count_ == capacity)
        {
            return capacity;
        }

        std::size_t start = 0;
        while (start < capacity && states_[start] == ProbeSlotState::Occupied)
        {
            ++start;
        }

        std::size_t longest = 0;
        std::size_t current = 0;
        for (std::size_t offset = 1; offset <= capacity; ++offset)
        {
            const std::size_t index = (start + offset) % capacity;
            if (states_[index] == ProbeSlotState::Occupied)
            {
                ++current;
                longest = current > longest ? current : longest;
                continue;
            }
            current = 0;
        }

        return longest;
    }

    /*
     * Purpose: Snapshot every live Target in slot order.
     * Design: Keeps slot storage private while supporting rehash and strategy migration.
     * Workflow: Append each occupied slot's Target into one vector.
     * Data Handoff: Returns a flat vector for rehashing, migration, or tests.
     */
    std::vector<Target> LinearProbing::entries() const
    {
        std::vector<Target> live;
        live.reserve(count_);
        for (std::size_t index = 0; index < slots_.size(); ++index)
        {
            if (states_[index] == ProbeSlotState::Occupied)
            {
                live.push_back(slots_[index]);
            }
        }
        return live;
    }

    /*
     * Purpose: Produce a per-slot view for display, one entry per occupied slot.
     * Design: Reuses HashBucketView so the session can render probing and chaining the same way.
     * Workflow: For each slot, emit its index with the occupied Target or no entries.
     * Data Handoff: Returns slot index and entry snapshots to the session/display layer.
     */
    std::vector<HashBucketView> LinearProbing::slotViews() const
    {
        std::vector<HashBucketView> views;
        views.reserve(slots_.size());
        for (std::size_t index = 0; index < slots_.size(); ++index)
        {
            HashBucketView view{index, {}};
            if (states_[index] == ProbeSlotState::Occupied)
            {
                view.entries.push_back(slots_[index]);
            }
            views.push_back(std::move(view));
        }
        return views;
    }
}
