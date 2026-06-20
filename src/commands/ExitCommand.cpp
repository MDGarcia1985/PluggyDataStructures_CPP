/*
 * File: ExitCommand.cpp
 * Description: Registers the application Exit command.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetProgram.h"
#include "registry/CommandRegistry.h"
#include "ui/Display.h"


namespace pds
{
    namespace
    {
        /*
         * Purpose: Request that the target command loop terminate.
         * Design: Changes state through TargetProgram instead of controlling the menu loop directly.
         * Workflow: Print the exit message and set the program's exit flag.
         * Data Handoff: Sends user exit intent into TargetProgram::requestExit().
         */
        void exitCommand(TargetProgram& program)
        {
            Display::printMessage("Exiting program.");
            program.requestExit();
        }
    }
}

LLB_REGISTER_EXIT_COMMAND(8, "Exit", pds::exitCommand)
