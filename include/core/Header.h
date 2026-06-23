/*
 * File: Header.h
 * Description: Shared standard library includes and project-wide constants.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

// Shared standard libraries and project-wide constants.
// Keep this file small: it should provide common utilities, not business logic.

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <filesystem>
#include <functional>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    inline constexpr const char* APP_NAME = "Pluggy Data Structure";
    inline constexpr const char* DEFAULT_DATA_FILE = "data/websites.txt";
    inline constexpr const char* DATA_DIRECTORY = "data/";
}
