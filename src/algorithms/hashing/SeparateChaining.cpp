/*
 * File: SeparateChaining.cpp
 * Description: Implements separate-chaining insert, lookup, remove, resize, and metrics.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/hashing/SeparateChaining.h"

#include "algorithms/common/KeyNormalization.h"
#include "algorithms/hashing/HashFunction.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    namespace
    {
        constexpr double MAX_LOAD_FACTOR = 0.75;
    }

    /*
     * Purpose: Create a chained table with a starting bucket count.
     * Design: Allocates at least one bucket so the modulo index is always valid.
     * Workflow: Size the bucket vector and start the element count at zero.
     * Data Handoff: Produces a ready-to-fill strategy for the hash-table facade or tests.
     */
    SeparateChaining::SeparateChaining(std::size_t initialBucketCount)
        : buckets_(initialBucketCount < 1 ? 1 : initialBucketCount), count_(0)
    {
    }

    /*
     * Purpose: Insert a Target, or update the value stored under an existing key.
     * Design: Hashes the first field to a bucket chain and resizes when the load factor is exceeded.
     * Workflow: Replace any same-key entry in the bucket, otherwise append, then rehash if needed.
     * Data Handoff: Receives a Target and stores it in the appropriate bucket chain.
     */
    void SeparateChaining::insert(const Target& target)
    {
        const std::string key = normalizeKey(target.fieldOne());
        std::vector<Target>& bucket = buckets_[bucketIndexFor(key, buckets_.size())];

        for (Target& entry : bucket)
        {
            if (normalizeKey(entry.fieldOne()) == key)
            {
                entry = target;
                return;
            }
        }

        bucket.push_back(target);
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
    bool SeparateChaining::lookup(const std::string& key, Target& found) const
    {
        const std::string normalized = normalizeKey(key);
        const std::vector<Target>& bucket = buckets_[bucketIndexFor(normalized, buckets_.size())];

        for (const Target& entry : bucket)
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
     * Design: Hashes to the owning bucket and erases only the matching chain entry.
     * Workflow: Compute the bucket, locate the match, and erase it from the chain.
     * Data Handoff: Receives a key and reports whether a matching entry was removed.
     */
    bool SeparateChaining::remove(const std::string& key)
    {
        const std::string normalized = normalizeKey(key);
        std::vector<Target>& bucket = buckets_[bucketIndexFor(normalized, buckets_.size())];

        for (std::size_t position = 0; position < bucket.size(); ++position)
        {
            if (normalizeKey(bucket[position].fieldOne()) == normalized)
            {
                bucket.erase(bucket.begin() + static_cast<std::ptrdiff_t>(position));
                --count_;
                return true;
            }
        }

        return false;
    }

    /*
     * Purpose: Grow or shrink the bucket array and redistribute all entries.
     * Design: Rebuilds chains for the new bucket count to keep lookups short as the table grows.
     * Workflow: Snapshot every entry, replace the bucket vector, and re-add each entry by new index.
     * Data Handoff: Preserves all stored Targets while changing only their bucket placement.
     */
    void SeparateChaining::rehash(std::size_t newBucketCount)
    {
        if (newBucketCount < 1)
        {
            newBucketCount = 1;
        }

        const std::vector<Target> allEntries = entries();
        buckets_.assign(newBucketCount, std::vector<Target>());
        count_ = 0;

        for (const Target& entry : allEntries)
        {
            const std::string key = normalizeKey(entry.fieldOne());
            buckets_[bucketIndexFor(key, buckets_.size())].push_back(entry);
            ++count_;
        }
    }

    /*
     * Purpose: Remove all entries while keeping the current bucket count.
     * Design: Clears each chain and resets the element count.
     * Workflow: Empty every bucket vector and zero the count.
     * Data Handoff: Leaves an empty strategy ready for reuse.
     */
    void SeparateChaining::clear()
    {
        for (std::vector<Target>& bucket : buckets_)
        {
            bucket.clear();
        }
        count_ = 0;
    }

    bool SeparateChaining::isEmpty() const
    {
        return count_ == 0;
    }

    std::size_t SeparateChaining::size() const
    {
        return count_;
    }

    std::size_t SeparateChaining::bucketCount() const
    {
        return buckets_.size();
    }

    double SeparateChaining::loadFactor() const
    {
        return static_cast<double>(count_) / static_cast<double>(buckets_.size());
    }

    /*
     * Purpose: Count entries that share a bucket with an earlier entry.
     * Design: Treats every entry beyond the first in a bucket as one collision.
     * Workflow: Sum (chainLength - 1) over every non-empty bucket.
     * Data Handoff: Supplies collision tooling and the session with a spread metric.
     */
    std::size_t SeparateChaining::collisionCount() const
    {
        std::size_t collisions = 0;
        for (const std::vector<Target>& bucket : buckets_)
        {
            if (bucket.size() > 1)
            {
                collisions += bucket.size() - 1;
            }
        }
        return collisions;
    }

    /*
     * Purpose: Report the length of the longest bucket chain.
     * Design: A direct indicator of worst-case lookup cost under chaining.
     * Workflow: Track the maximum chain length while scanning buckets.
     * Data Handoff: Supplies collision tooling and the session with a worst-case metric.
     */
    std::size_t SeparateChaining::longestChain() const
    {
        std::size_t longest = 0;
        for (const std::vector<Target>& bucket : buckets_)
        {
            if (bucket.size() > longest)
            {
                longest = bucket.size();
            }
        }
        return longest;
    }

    /*
     * Purpose: Snapshot every stored Target across all buckets.
     * Design: Keeps bucket storage private while supporting rehash and strategy migration.
     * Workflow: Append each bucket's entries into one vector in bucket order.
     * Data Handoff: Returns a flat vector for rehashing, migration, or tests.
     */
    std::vector<Target> SeparateChaining::entries() const
    {
        std::vector<Target> all;
        all.reserve(count_);
        for (const std::vector<Target>& bucket : buckets_)
        {
            for (const Target& entry : bucket)
            {
                all.push_back(entry);
            }
        }
        return all;
    }

    /*
     * Purpose: Produce a per-bucket view of stored entries for display.
     * Design: Snapshots each bucket chain so display avoids touching internals.
     * Workflow: Walk each bucket, copying its chain into an indexed view.
     * Data Handoff: Returns bucket index and entry snapshots to the session/display layer.
     */
    std::vector<HashBucketView> SeparateChaining::bucketViews() const
    {
        std::vector<HashBucketView> views;
        views.reserve(buckets_.size());
        for (std::size_t index = 0; index < buckets_.size(); ++index)
        {
            views.push_back(HashBucketView{index, buckets_[index]});
        }
        return views;
    }
}
