/*
 * File: SortSupport.h
 * Description: Declares shared generic sorting execution support.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/TargetProgram.h"


// named container llb = linked list browser
namespace llb
{
    using SortFunction = void (*)(std::vector<Target>&);

    bool targetLess(const Target& left, const Target& right);
    void runSortCommand(
        TargetProgram& program,
        const std::string& algorithmName,
        SortFunction sortFunction);
}
