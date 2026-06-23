/*
 * File: DeleteTargetCommand.cpp
 * Description: Registers the delete-target command.
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
         * Purpose: Delete a user-selected target through the active program.
         * Design: Adapts TargetProgram::deleteTargetFromUser() to the command signature.
         * Workflow: Receive the program and invoke its delete-target workflow.
         * Data Handoff: Routes command execution into TargetProgram-owned selection and list state.
         */
        void deleteTargetCommand(TargetProgram& program)
        {
            program.deleteTargetFromUser();
        }
    }
}

PDS_REGISTER_COMMAND(5, "Delete a target from the list", pds::deleteTargetCommand)
