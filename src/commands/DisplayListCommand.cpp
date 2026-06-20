/*
 * File: DisplayListCommand.cpp
 * Description: Registers the target-list display command.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetProgram.h"
#include "registry/CommandRegistry.h"


namespace pds
{
    namespace
    {
        /*
         * Purpose: Display all records in the active target list.
         * Design: Adapts TargetProgram::displayList() to the registry command signature.
         * Workflow: Receive the active program and call its display operation.
         * Data Handoff: Passes command control into TargetProgram without owning list data.
         */
        void displayListCommand(TargetProgram& program)
        {
            program.displayList();
        }
    }
}

LLB_REGISTER_COMMAND(1, "Display the list", pds::displayListCommand)
