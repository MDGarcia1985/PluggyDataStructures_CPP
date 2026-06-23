/*
 * File: SortListCommand.cpp
 * Description: Registers the command that opens the Sort Type Menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetProgram.h"
#include "registry/CommandRegistry.h"
#include "ui/SortTypeMenu.h"


namespace pds
{
    namespace
    {
        /*
         * Purpose: Open sorting choices for the active target list.
         * Design: Keeps the main command unaware of registered algorithm details.
         * Workflow: Receive the active program and pass it to SortTypeMenu::run().
         * Data Handoff: Routes the current TargetProgram from the target menu into sorting navigation.
         */
        void sortListCommand(TargetProgram& program)
        {
            SortTypeMenu::run(program);
        }
    }
}

PDS_REGISTER_COMMAND(7, "Sort list", pds::sortListCommand)
