/*
 * File: TargetOrdering.h
 * Description: Declares the shared ascending ordering used to compare Target records.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Target.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    bool targetLess(const Target& left, const Target& right);
    bool targetEqualsByOrdering(const Target& left, const Target& right);
}
