/*
 * File: MoveForwardCommand.cpp
 * Description: Registers the forward-navigation command.
 */

#include "core/TargetProgram.h"
#include "registry/CommandRegistry.h"


namespace llb
{
    namespace
    {
        /*
         * Purpose: Move to and display the next active target.
         * Design: Adapts TargetProgram::goForward() to the registry command signature.
         * Workflow: Receive the program and invoke its forward-navigation operation.
         * Data Handoff: Passes user intent into TargetProgram's linked-list workflow.
         */
        void moveForwardCommand(TargetProgram& program)
        {
            program.goForward();
        }
    }
}

LLB_REGISTER_COMMAND(2, "Go forward and display the current target", llb::moveForwardCommand)
