/*
 * File: HashFunction.h
 * Description: Declares normalized string hashing and bucket-index mapping for hash tables.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>
#include <string>


// named container pds = Pluggy Data Structures
namespace pds
{
    std::size_t hashKey(const std::string& key);
    std::size_t bucketIndexFor(const std::string& key, std::size_t bucketCount);
}
