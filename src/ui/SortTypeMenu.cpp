/*
 * File: SortTypeMenu.cpp
 * Description: Implements navigation for registered sorting algorithms.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetProgram.h"
#include "registry/SortRegistry.h"
#include "ui/Menu.h"
#include "ui/SortTypeMenu.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Display registered sort commands and execute the selected algorithm.
     * Design: Reads all options, including Exit, from SortRegistry instead of hardcoded menu logic.
     * Workflow: Build labels, select a valid item, return for Exit, or invoke the algorithm action.
     * Data Handoff: Routes the active TargetProgram into the selected sorting command.
     */
    void SortTypeMenu::run(TargetProgram& program)
    {
        const std::vector<SortCommand> commands = SortRegistry::instance().commands();
        std::vector<std::string> labels;
        labels.reserve(commands.size());

        for (const SortCommand& command : commands)
        {
            labels.push_back(command.label);
        }

        const std::size_t selectedIndex = Menu::select("Sort Type Menu", labels);
        if (commands[selectedIndex].isExit)
        {
            return;
        }

        commands[selectedIndex].action(program);
    }
}
