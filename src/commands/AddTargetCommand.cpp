/*
 * File: AddTargetCommand.cpp
 * Description: Registers the add-target command.
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
         * Purpose: Collect and append a target through the active program.
         * Design: Adapts TargetProgram::addTargetFromUser() to the command signature.
         * Workflow: Receive the program and invoke its add-target workflow.
         * Data Handoff: Routes command execution into TargetProgram-owned input and list state.
         */
        void addTargetCommand(TargetProgram& program)
        {
            program.addTargetFromUser();
        }
    }
}

LLB_REGISTER_COMMAND(4, "Add another target to the list", pds::addTargetCommand)
