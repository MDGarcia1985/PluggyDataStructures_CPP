/*
 * File: TargetDataStructureMenu.h
 * Description: Declares navigation for registered target-list commands.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace llb
{
    class TargetProgram;

    class TargetDataStructureMenu
    {
    public:
        static void run(TargetProgram& program);
    };
}
