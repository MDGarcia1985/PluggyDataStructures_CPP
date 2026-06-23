/*
 * File: TargetOrdering.cpp
 * Description: Implements the shared three-tier ascending ordering for Target records.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "algorithms/common/TargetOrdering.h"

#include "algorithms/common/KeyNormalization.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Compare generic Target records for ascending sort order.
     * Design: Uses case-insensitive field-one ordering, then field two, with raw text as a stable tie-breaker.
     * Workflow: Normalize fields, compare primary and secondary values, then compare original display text.
     * Data Handoff: Receives two Targets from an algorithm and returns their strict relative ordering.
     */
    bool targetLess(const Target& left, const Target& right)
    {
        const std::string leftFirst = normalizeKey(left.fieldOne());
        const std::string rightFirst = normalizeKey(right.fieldOne());
        if (leftFirst != rightFirst)
        {
            return leftFirst < rightFirst;
        }

        const std::string leftSecond = normalizeKey(left.fieldTwo());
        const std::string rightSecond = normalizeKey(right.fieldTwo());
        if (leftSecond != rightSecond)
        {
            return leftSecond < rightSecond;
        }

        return left.toDisplayString() < right.toDisplayString();
    }

    /*
     * Purpose: Decide whether two Targets are equal under the shared ordering.
     * Design: Defines equality as neither value being strictly less than the other.
     * Workflow: Evaluate targetLess in both directions and report mutual non-precedence.
     * Data Handoff: Supports duplicate rejection and node matching for ordered structures.
     */
    bool targetEqualsByOrdering(const Target& left, const Target& right)
    {
        return !targetLess(left, right) && !targetLess(right, left);
    }
}
