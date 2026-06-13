/*
 * File: Display.h
 * Description: Declares console output helpers for menus, lists, and search results.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/Target.h"
#include "core/TargetList.h"


// named container llb = linked list browser
namespace llb
{
    namespace Display
    {
        void printTitle();
        void printMessage(const std::string& message);
        void printTarget(const Target& target, std::size_t position = 0);
        void printTargetList(const std::vector<Target>& targets);
        void printCurrentTarget(const Target* target, std::size_t position);
        void printSearchResults(const std::vector<TargetSearchResult>& results);
    }
}
