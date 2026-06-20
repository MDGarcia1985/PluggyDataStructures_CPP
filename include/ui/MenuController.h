/*
 * File: MenuController.h
 * Description: Declares the generic menu-label adapter that runs registered session operations.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "ui/Menu.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Drive any structure's menu by converting registry items into labels and running them.
     * Design: Parameterized on the registry and session so one controller serves every data structure.
     * Workflow: Read ordered operations, show labels, run the chosen action, and stop on an Exit item.
     * Data Handoff: Routes the active SessionT into the selected operation's action.
     */
    template <typename RegistryT, typename SessionT>
    class MenuController
    {
    public:
        static void run(const std::string& title, SessionT& session)
        {
            while (true)
            {
                const auto operations = RegistryT::instance().operations();

                std::vector<std::string> labels;
                labels.reserve(operations.size());
                for (const auto& operation : operations)
                {
                    labels.push_back(operation.label);
                }

                const std::size_t selectedIndex = Menu::select(title, labels);
                const auto& chosen = operations[selectedIndex];

                if (chosen.isExit)
                {
                    if (chosen.action)
                    {
                        chosen.action(session);
                    }
                    return;
                }

                chosen.action(session);
            }
        }
    };
}
