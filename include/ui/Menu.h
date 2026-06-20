/*
 * File: Menu.h
 * Description: Declares reusable console menu display and input behavior.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    class Menu
    {
    public:
        static void display(const std::string& title, const std::vector<std::string>& options);
        static std::size_t select(const std::string& title, const std::vector<std::string>& options);
        static std::size_t promptSelection(std::size_t optionCount);
        static int promptChoice();
        static int promptInteger(const std::string& prompt);
        static void printInvalidSelection();
    };
}
