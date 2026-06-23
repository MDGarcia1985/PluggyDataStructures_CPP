/*
 * File: QuickSort.h
 * Description: Declares the pure quick sort over a vector of Target records.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Target.h"

#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    void quickSort(std::vector<Target>& targets);
}
