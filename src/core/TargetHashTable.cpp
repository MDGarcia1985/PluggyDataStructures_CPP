/*
 * File: TargetHashTable.cpp
 * Description: Implements separate-chaining hashing, lookup, erase, and load-factor resizing.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetHashTable.h"


// named container llb = linked list browser
namespace llb
{
    namespace
    {
        constexpr double MAX_LOAD_FACTOR = 0.75;
    }

    /*
     * Purpose: Create a hash table with a starting bucket count.
     * Design: Allocates at least one bucket so the modulo index is always valid.
     * Workflow: Size the bucket vector and start the element count at zero.
     * Data Handoff: Produces a ready-to-fill table for a HashTableSession or tests.
     */
    TargetHashTable::TargetHashTable(std::size_t initialBuckets)
        : buckets_(initialBuckets < 1 ? 1 : initialBuckets), count_(0)
    {
    }

    /*
     * Purpose: Insert a Target, or update the value stored under an existing key.
     * Design: Hashes the first field to a bucket chain and resizes when the load factor is exceeded.
     * Workflow: Replace any same-key entry in the bucket, otherwise append, then rehash if needed.
     * Data Handoff: Receives a Target and stores it in the appropriate bucket chain.
     */
    void TargetHashTable::insert(const Target& target)
    {
        const std::string key = normalizeKey(target.fieldOne());
        const std::size_t index = bucketIndexFor(key);

        TargetList& bucket = buckets_[index];
        const std::vector<Target> entries = bucket.toVector();
        for (std::size_t position = 0; position < entries.size(); ++position)
        {
            if (normalizeKey(entries[position].fieldOne()) == key)
            {
                bucket.removeAt(position + 1);
                bucket.addBack(target);
                return;
            }
        }

        bucket.addBack(target);
        ++count_;

        if (loadFactor() > MAX_LOAD_FACTOR)
        {
            rehash(buckets_.size() * 2);
        }
    }

    /*
     * Purpose: Find a stored Target by its first-field key.
     * Design: Hashes to the owning bucket and scans only that short chain.
     * Workflow: Compute the bucket, compare normalized keys, and copy the first match out.
     * Data Handoff: Writes the matched Target into found and reports whether a match existed.
     */
    bool TargetHashTable::find(const std::string& key, Target& found) const
    {
        const std::string normalized = normalizeKey(key);
        const std::size_t index = bucketIndexFor(normalized);

        for (const Target& entry : buckets_[index].toVector())
        {
            if (normalizeKey(entry.fieldOne()) == normalized)
            {
                found = entry;
                return true;
            }
        }

        return false;
    }

    /*
     * Purpose: Remove a stored Target by its first-field key.
     * Design: Hashes to the owning bucket and removes only the matching chain node.
     * Workflow: Compute the bucket, locate the matching position, and remove it.
     * Data Handoff: Receives a key and reports whether a matching entry was removed.
     */
    bool TargetHashTable::erase(const std::string& key)
    {
        const std::string normalized = normalizeKey(key);
        const std::size_t index = bucketIndexFor(normalized);

        TargetList& bucket = buckets_[index];
        const std::vector<Target> entries = bucket.toVector();
        for (std::size_t position = 0; position < entries.size(); ++position)
        {
            if (normalizeKey(entries[position].fieldOne()) == normalized)
            {
                bucket.removeAt(position + 1);
                --count_;
                return true;
            }
        }

        return false;
    }

    /*
     * Purpose: Report whether the table holds no entries.
     * Design: Uses the cached element count for constant-time access.
     * Workflow: Compare count_ to zero.
     * Data Handoff: Returns a boolean to callers before lookups.
     */
    bool TargetHashTable::isEmpty() const
    {
        return count_ == 0;
    }

    /*
     * Purpose: Report how many entries are stored.
     * Design: Returns the cached element count for constant-time access.
     * Workflow: Read count_ directly.
     * Data Handoff: Gives callers the element total for load-factor and display logic.
     */
    std::size_t TargetHashTable::size() const
    {
        return count_;
    }

    /*
     * Purpose: Report how many buckets the table currently allocates.
     * Design: Returns the bucket vector size for constant-time access.
     * Workflow: Read the bucket vector size.
     * Data Handoff: Gives callers the divisor used for load-factor and display logic.
     */
    std::size_t TargetHashTable::bucketCount() const
    {
        return buckets_.size();
    }

    /*
     * Purpose: Report the current load factor.
     * Design: Defines load factor as entries divided by buckets.
     * Workflow: Divide the element count by the bucket count as doubles.
     * Data Handoff: Supplies insert() and display with the resize threshold metric.
     */
    double TargetHashTable::loadFactor() const
    {
        return static_cast<double>(count_) / static_cast<double>(buckets_.size());
    }

    /*
     * Purpose: Produce a per-bucket view of stored entries for display.
     * Design: Snapshots each bucket chain into a vector so display avoids node internals.
     * Workflow: Walk each bucket, copying its chain into an indexed view.
     * Data Handoff: Returns bucket index and entry snapshots to the display layer.
     */
    std::vector<HashBucketView> TargetHashTable::bucketViews() const
    {
        std::vector<HashBucketView> views;
        views.reserve(buckets_.size());
        for (std::size_t index = 0; index < buckets_.size(); ++index)
        {
            views.push_back(HashBucketView{index, buckets_[index].toVector()});
        }
        return views;
    }

    /*
     * Purpose: Normalize a key for case-insensitive hashing and comparison.
     * Design: Works on a copy so stored Target values are never altered.
     * Workflow: Lowercase each byte safely and return the converted text.
     * Data Handoff: Supplies hashing and matching with comparable key text.
     */
    std::string TargetHashTable::normalizeKey(const std::string& key)
    {
        std::string normalized = key;
        std::transform(normalized.begin(), normalized.end(), normalized.begin(),
            [](unsigned char character)
            {
                return static_cast<char>(std::tolower(character));
            });
        return normalized;
    }

    /*
     * Purpose: Map a normalized key to a bucket index.
     * Design: Uses the standard string hash reduced modulo the bucket count.
     * Workflow: Hash the normalized key and take the remainder over the bucket count.
     * Data Handoff: Supplies insert, find, and erase with the owning bucket index.
     */
    std::size_t TargetHashTable::bucketIndexFor(const std::string& key) const
    {
        return std::hash<std::string>{}(key) % buckets_.size();
    }

    /*
     * Purpose: Grow the bucket array and redistribute all entries.
     * Design: Rebuilds chains for the new bucket count to keep lookups short as the table grows.
     * Workflow: Snapshot every entry, replace the bucket vector, and re-add each entry by new index.
     * Data Handoff: Preserves all stored Targets while changing only their bucket placement.
     */
    void TargetHashTable::rehash(std::size_t newBucketCount)
    {
        if (newBucketCount < 1)
        {
            newBucketCount = 1;
        }

        std::vector<Target> allEntries;
        allEntries.reserve(count_);
        for (const TargetList& bucket : buckets_)
        {
            for (const Target& entry : bucket.toVector())
            {
                allEntries.push_back(entry);
            }
        }

        buckets_.assign(newBucketCount, TargetList());
        for (const Target& entry : allEntries)
        {
            const std::size_t index = bucketIndexFor(normalizeKey(entry.fieldOne()));
            buckets_[index].addBack(entry);
        }
    }
}
