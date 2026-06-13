/*
 * File: TargetDataStructureMenu.cpp
 * Description: Implements navigation for registered target-list commands.
 */

#include "core/TargetProgram.h"
#include "registry/CommandRegistry.h"
#include "ui/Menu.h"
#include "ui/TargetDataStructureMenu.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Run the target data structure command menu until Exit is requested.
     * Design: Converts registry entries to labels while keeping execution in the controller.
     * Workflow: Fetch ordered commands, select a valid position, execute its action, and repeat.
     * Data Handoff: Routes the active TargetProgram into the selected registered command.
     */
    void TargetDataStructureMenu::run(TargetProgram& program)
    {
        while (!program.exitRequested())
        {
            const std::vector<CommandPlugin> commands = CommandRegistry::instance().commands();
            std::vector<std::string> labels;
            labels.reserve(commands.size());

            for (const CommandPlugin& command : commands)
            {
                labels.push_back(command.label);
            }

            const std::size_t selectedIndex = Menu::select("Target Data Structure Menu", labels);
            commands[selectedIndex].action(program);
        }
    }
}
