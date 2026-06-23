/*
 * File: TargetHashTable.cpp
 * Description: Implements the hash-table facade that delegates to the active collision strategy.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "structures/TargetHashTable.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Create a hash table that starts in separate-chaining mode.
     * Design: Sizes both strategy objects so a later switch reuses the same capacity hint.
     * Workflow: Construct each strategy with the initial bucket count and select chaining.
     * Data Handoff: Produces a ready-to-fill table for a HashTableSession or tests.
     */
    TargetHashTable::TargetHashTable(std::size_t initialBuckets)
        : strategy_(HashStrategy::SeparateChaining),
          chaining_(initialBuckets),
          probing_(initialBuckets)
    {
    }

    /*
     * Purpose: Insert or update a Target through the active strategy.
     * Design: Keeps the facade free of hashing detail by delegating to the chosen strategy.
     * Workflow: Route the Target to the active strategy's insert.
     * Data Handoff: Hands the Target to either chaining or probing storage.
     */
    void TargetHashTable::insert(const Target& target)
    {
        if (strategy_ == HashStrategy::SeparateChaining)
        {
            chaining_.insert(target);
        }
        else
        {
            probing_.insert(target);
        }
    }

    /*
     * Purpose: Find a Target by key through the active strategy.
     * Design: Delegates lookup so the facade exposes one API across both strategies.
     * Workflow: Route the key to the active strategy's lookup.
     * Data Handoff: Writes the matched Target into found and reports success.
     */
    bool TargetHashTable::find(const std::string& key, Target& found) const
    {
        if (strategy_ == HashStrategy::SeparateChaining)
        {
            return chaining_.lookup(key, found);
        }
        return probing_.lookup(key, found);
    }

    /*
     * Purpose: Remove a Target by key through the active strategy.
     * Design: Delegates removal so tombstone or chain logic stays in the strategy.
     * Workflow: Route the key to the active strategy's remove.
     * Data Handoff: Reports whether a matching entry was removed.
     */
    bool TargetHashTable::erase(const std::string& key)
    {
        if (strategy_ == HashStrategy::SeparateChaining)
        {
            return chaining_.remove(key);
        }
        return probing_.remove(key);
    }

    bool TargetHashTable::isEmpty() const
    {
        return size() == 0;
    }

    std::size_t TargetHashTable::size() const
    {
        return strategy_ == HashStrategy::SeparateChaining ? chaining_.size() : probing_.size();
    }

    /*
     * Purpose: Report the slot count of the active strategy.
     * Design: Maps both strategies onto one "bucket count" notion for display and load factor.
     * Workflow: Return the chaining bucket count or the probing capacity.
     * Data Handoff: Supplies the session with the divisor used in load-factor display.
     */
    std::size_t TargetHashTable::bucketCount() const
    {
        return strategy_ == HashStrategy::SeparateChaining ? chaining_.bucketCount() : probing_.capacity();
    }

    double TargetHashTable::loadFactor() const
    {
        return strategy_ == HashStrategy::SeparateChaining ? chaining_.loadFactor() : probing_.loadFactor();
    }

    /*
     * Purpose: Produce a per-slot view of the active strategy for display.
     * Design: Reuses HashBucketView so the session renders chaining and probing the same way.
     * Workflow: Return chaining bucket views or probing slot views.
     * Data Handoff: Returns indexed entry snapshots to the session/display layer.
     */
    std::vector<HashBucketView> TargetHashTable::bucketViews() const
    {
        return strategy_ == HashStrategy::SeparateChaining ? chaining_.bucketViews() : probing_.slotViews();
    }

    HashStrategy TargetHashTable::strategy() const
    {
        return strategy_;
    }

    std::string TargetHashTable::strategyName() const
    {
        return strategy_ == HashStrategy::SeparateChaining ? "Separate Chaining" : "Linear Probing";
    }

    /*
     * Purpose: Switch the collision strategy while preserving stored entries.
     * Design: Migrates entries from the active strategy into the target strategy, then clears the old one.
     * Workflow: Return early if unchanged; otherwise snapshot, reload the target, clear the source, and switch.
     * Data Handoff: Moves every stored Target from one strategy's storage into the other.
     */
    void TargetHashTable::useStrategy(HashStrategy strategy)
    {
        if (strategy == strategy_)
        {
            return;
        }

        if (strategy == HashStrategy::LinearProbing)
        {
            probing_.clear();
            for (const Target& entry : chaining_.entries())
            {
                probing_.insert(entry);
            }
            chaining_.clear();
        }
        else
        {
            chaining_.clear();
            for (const Target& entry : probing_.entries())
            {
                chaining_.insert(entry);
            }
            probing_.clear();
        }

        strategy_ = strategy;
    }

    /*
     * Purpose: Report collisions for the active strategy.
     * Design: Uses chain overflow for chaining and probe displacement for probing.
     * Workflow: Return the chaining collision count or the probing displacement past one probe each.
     * Data Handoff: Supplies the session with a strategy-appropriate collision metric.
     */
    std::size_t TargetHashTable::collisionCount() const
    {
        if (strategy_ == HashStrategy::SeparateChaining)
        {
            return chaining_.collisionCount();
        }
        const std::size_t total = probing_.totalProbeCount();
        return total > probing_.size() ? total - probing_.size() : 0;
    }

    /*
     * Purpose: Report the worst-case run length for the active strategy.
     * Design: Maps to longest chain under chaining and longest cluster under probing.
     * Workflow: Return the matching strategy metric.
     * Data Handoff: Supplies the session with a worst-case lookup indicator.
     */
    std::size_t TargetHashTable::longestRun() const
    {
        return strategy_ == HashStrategy::SeparateChaining ? chaining_.longestChain() : probing_.longestCluster();
    }

    /*
     * Purpose: Report the average probe length for the active strategy.
     * Design: Uses true probe distance for probing and an approximate chain cost for chaining.
     * Workflow: Divide probe or collision totals by the element count, guarding the empty case.
     * Data Handoff: Supplies the session with an average lookup-cost metric.
     */
    double TargetHashTable::averageProbeLength() const
    {
        const std::size_t entryCount = size();
        if (entryCount == 0)
        {
            return 0.0;
        }

        if (strategy_ == HashStrategy::LinearProbing)
        {
            return static_cast<double>(probing_.totalProbeCount()) / static_cast<double>(entryCount);
        }

        return 1.0 + static_cast<double>(chaining_.collisionCount()) / static_cast<double>(entryCount);
    }
}
