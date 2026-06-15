/*
 * File: StructureMenu.h
 * Description: Declares the top-level menu that selects which data structure to explore.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"


// named container llb = linked list browser
namespace llb
{
    class StructureMenu
    {
    public:
        static void run(const std::string& dataFilePath);
    };
}
