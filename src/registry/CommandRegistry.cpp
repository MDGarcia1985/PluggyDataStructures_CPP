/*
 * File: CommandRegistry.cpp
 * Description: Implements the shared target-menu command registry singleton.
 */

#include "registry/CommandRegistry.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Access the shared target command registry.
     * Design: Uses a function-local static object for reliable one-time initialization.
     * Workflow: Create the registry on first access and return the same reference thereafter.
     * Data Handoff: Gives independent command modules shared registration storage.
     */
    CommandRegistry& CommandRegistry::instance()
    {
        static CommandRegistry registry;
        return registry;
    }
}
