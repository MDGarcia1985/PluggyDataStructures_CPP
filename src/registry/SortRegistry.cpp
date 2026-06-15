/*
 * File: SortRegistry.cpp
 * Description: Implements sorting algorithm registration and its registry-owned Exit entry.
 */

#include "registry/SortRegistry.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Create the sort registry with its built-in Exit entry.
     * Design: Stores Exit as registry data so SortTypeMenu does not hardcode an extra command.
     * Workflow: Seed the marked Exit item during registry construction.
     * Data Handoff: Seeds shared sort-menu storage before algorithm modules register.
     */
    SortRegistry::SortRegistry()
    {
        seedItem(SortCommand{0, "Exit", std::function<void(TargetProgram&)>(), true});
    }

    /*
     * Purpose: Access the shared sorting algorithm registry.
     * Design: Uses a function-local static object for one-time construction.
     * Workflow: Create the registry on first access and return its shared reference.
     * Data Handoff: Gives algorithm modules and SortTypeMenu access to the same registrations.
     */
    SortRegistry& SortRegistry::instance()
    {
        static SortRegistry registry;
        return registry;
    }
}
