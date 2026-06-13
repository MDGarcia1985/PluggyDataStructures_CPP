/*
 * File: SortRegistry.cpp
 * Description: Implements sorting algorithm registration and Exit-last ordering.
 */

#include "registry/SortRegistry.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Create the sort registry with its built-in Exit entry.
     * Design: Stores Exit as registry data so SortTypeMenu does not hardcode an extra command.
     * Workflow: Append the marked Exit item during registry construction.
     * Data Handoff: Seeds shared sort-menu storage before algorithm modules register.
     */
    SortRegistry::SortRegistry()
    {
        append(SortCommand{"Exit", std::function<void(TargetProgram&)>(), true});
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

    /*
     * Purpose: Register one sorting algorithm.
     * Design: Rejects empty labels, missing actions, Exit registrations, and duplicate labels.
     * Workflow: Validate the command, scan stored entries, and append an accepted algorithm.
     * Data Handoff: Receives sort metadata from an algorithm module and stores it in RegistryBase.
     */
    bool SortRegistry::registerCommand(SortCommand command)
    {
        if (command.label.empty() || !command.action || command.isExit)
        {
            return false;
        }

        for (const SortCommand& existingCommand : storedItems())
        {
            if (existingCommand.label == command.label)
            {
                return false;
            }
        }

        return append(std::move(command));
    }

    /*
     * Purpose: Return sort commands with the Exit entry at the end.
     * Design: Uses stable partitioning so algorithm registration order is preserved.
     * Workflow: Copy stored commands, move marked Exit entries behind algorithms, and return the copy.
     * Data Handoff: Supplies SortTypeMenu with ordered labels and actions.
     */
    std::vector<SortCommand> SortRegistry::commands() const
    {
        std::vector<SortCommand> orderedCommands = storedItems();
        std::stable_partition(orderedCommands.begin(), orderedCommands.end(),
            [](const SortCommand& command)
            {
                return !command.isExit;
            });
        return orderedCommands;
    }
}
