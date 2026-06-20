/*
 * File: MainMenu.h
 * Description: Declares the top-level application menu flow.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace pds
{
    class TargetProgram;

    class MainMenu
    {
    public:
        static void run(TargetProgram& program);
    };
}
