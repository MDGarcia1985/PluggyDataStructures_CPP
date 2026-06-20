/*
 * File: FindTargetCommand.cpp
 * Description: Registers the find-target command.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetProgram.h"
#include "registry/CommandRegistry.h"


namespace llb
{
    namespace
    {
        /*
         * Purpose: Search the active target list using user input.
         * Design: Adapts TargetProgram::findTargetFromUser() to the command signature.
         * Workflow: Receive the program and invoke its search workflow.
         * Data Handoff: Routes command execution into TargetProgram-owned search and selection state.
         */
        void findTargetCommand(TargetProgram& program)
        {
            program.findTargetFromUser();
        }
    }
}

LLB_REGISTER_COMMAND(6, "Find a target in the list", llb::findTargetCommand)
