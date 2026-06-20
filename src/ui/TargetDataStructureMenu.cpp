/*
 * File: TargetDataStructureMenu.cpp
 * Description: Implements navigation for registered target-list commands.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetProgram.h"
#include "registry/CommandRegistry.h"
#include "ui/MenuController.h"
#include "ui/TargetDataStructureMenu.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Run the target data structure command menu until Exit is requested.
     * Design: Delegates rendering and dispatch to the generic MenuController over CommandRegistry.
     * Workflow: Forward the active program to the shared controller using the command registry.
     * Data Handoff: Routes the active TargetProgram into the selected registered command.
     */
    void TargetDataStructureMenu::run(TargetProgram& program)
    {
        MenuController<CommandRegistry, TargetProgram>::run("Target Data Structure Menu", program);
    }
}
