/*
 * File: CommandRegistry.cpp
 * Description: Implements target-menu command registration and Exit-last ordering.
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

    /*
     * Purpose: Register a valid command for the Target Data Structure Menu.
     * Design: Rejects incomplete entries, duplicate IDs, and multiple Exit commands before storage.
     * Workflow: Validate fields, scan stored items for conflicts, and append an accepted command.
     * Data Handoff: Receives command metadata from a command module and stores it in RegistryBase.
     */
    bool CommandRegistry::registerCommand(CommandPlugin command)
    {
        if (command.id < 1 || command.label.empty() || !command.action)
        {
            return false;
        }

        for (const CommandPlugin& existingCommand : storedItems())
        {
            if (existingCommand.id == command.id || (existingCommand.isExit && command.isExit))
            {
                return false;
            }
        }

        return append(std::move(command));
    }

    /*
     * Purpose: Return commands in stable menu order with Exit at the end.
     * Design: Sorts a copy by Exit status and registration ID without changing registry storage.
     * Workflow: Copy stored commands, order regular commands by ID, and place Exit last.
     * Data Handoff: Supplies TargetDataStructureMenu with its executable menu entries.
     */
    std::vector<CommandPlugin> CommandRegistry::commands() const
    {
        std::vector<CommandPlugin> orderedCommands = storedItems();
        std::sort(orderedCommands.begin(), orderedCommands.end(),
            [](const CommandPlugin& left, const CommandPlugin& right)
            {
                if (left.isExit != right.isExit)
                {
                    return !left.isExit;
                }
                return left.id < right.id;
            });
        return orderedCommands;
    }

    /*
     * Purpose: Find a registered target command by its stable ID.
     * Design: Returns nullptr for absent IDs instead of throwing or exposing mutable storage.
     * Workflow: Scan stored registrations and return the matching address when found.
     * Data Handoff: Gives tests and non-menu callers read-only access to one command.
     */
    const CommandPlugin* CommandRegistry::findById(int id) const
    {
        for (const CommandPlugin& command : storedItems())
        {
            if (command.id == id)
            {
                return &command;
            }
        }

        return nullptr;
    }
}
