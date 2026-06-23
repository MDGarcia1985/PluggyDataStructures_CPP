/*
 * File: KeyNormalization.h
 * Description: Declares shared case-insensitive key normalization for keyed structures.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>


// named container pds = Pluggy Data Structures
namespace pds
{
    std::string normalizeKey(std::string text);
}
