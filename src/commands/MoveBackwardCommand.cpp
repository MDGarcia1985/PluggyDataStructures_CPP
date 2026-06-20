/*
 * File: MoveBackwardCommand.cpp
 * Description: Registers the backward-navigation command.
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
         * Purpose: Move to and display the previous active target.
         * Design: Adapts TargetProgram::goBackward() to the registry command signature.
         * Workflow: Receive the program and invoke its backward-navigation operation.
         * Data Handoff: Passes user intent into TargetProgram's linked-list workflow.
         */
        void moveBackwardCommand(TargetProgram& program)
        {
            program.goBackward();
        }
    }
}

LLB_REGISTER_COMMAND(3, "Go backward and display the current target", llb::moveBackwardCommand)
