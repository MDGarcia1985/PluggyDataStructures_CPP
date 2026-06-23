/*
 * File: CollisionAnalysis.cpp
 * Description: Implements collision and probe metrics shared across hashing strategies.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/hashing/CollisionAnalysis.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Report how many chained entries collided into shared buckets.
     * Design: Delegates to the strategy so the metric stays consistent with its own accounting.
     * Workflow: Return the strategy's collision count.
     * Data Handoff: Supplies the session with a chaining spread metric.
     */
    std::size_t collisionCount(const SeparateChaining& table)
    {
        return table.collisionCount();
    }

    /*
     * Purpose: Report the longest bucket chain length under chaining.
     * Design: Delegates to the strategy's own scan for a single source of truth.
     * Workflow: Return the strategy's longest chain.
     * Data Handoff: Supplies the session with a chaining worst-case metric.
     */
    std::size_t longestChain(const SeparateChaining& table)
    {
        return table.longestChain();
    }

    double loadFactor(const SeparateChaining& table)
    {
        return table.loadFactor();
    }

    /*
     * Purpose: Report how many probed entries sit beyond their home slot.
     * Design: Approximates open-addressing collisions as total displacement past the first probe.
     * Workflow: Subtract one ideal probe per entry from the total probe count.
     * Data Handoff: Supplies the session with a probing collision metric.
     */
    std::size_t collisionCount(const LinearProbing& table)
    {
        const std::size_t total = table.totalProbeCount();
        const std::size_t size = table.size();
        return total > size ? total - size : 0;
    }

    /*
     * Purpose: Report the longest run of consecutive occupied slots under probing.
     * Design: Delegates to the strategy's own cluster scan.
     * Workflow: Return the strategy's longest cluster.
     * Data Handoff: Supplies the session with a probing clustering metric.
     */
    std::size_t longestCluster(const LinearProbing& table)
    {
        return table.longestCluster();
    }

    /*
     * Purpose: Report the average number of probes per live entry.
     * Design: Divides total probe distance by the number of stored entries.
     * Workflow: Guard the empty case, then divide total probes by size.
     * Data Handoff: Supplies the session with an average lookup-cost metric.
     */
    double averageProbeLength(const LinearProbing& table)
    {
        if (table.size() == 0)
        {
            return 0.0;
        }
        return static_cast<double>(table.totalProbeCount()) / static_cast<double>(table.size());
    }

    double loadFactor(const LinearProbing& table)
    {
        return table.loadFactor();
    }
}
