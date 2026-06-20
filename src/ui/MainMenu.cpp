/*
 * File: MainMenu.cpp
 * Description: Implements the top-level application menu flow.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetProgram.h"
#include "ui/DataSourceMenu.h"
#include "ui/Display.h"
#include "ui/MainMenu.h"
#include "ui/StructureMenu.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Start dataset selection and hand control to the data structure menu.
     * Design: Coordinates menu controllers while leaving loading mechanics in TargetProgram.
     * Workflow: Print the title, choose a dataset, report its size, and open the structure menu.
     * Data Handoff: Passes the selected dataset path into the top-level StructureMenu.
     */
    void MainMenu::run(TargetProgram& program)
    {
        Display::printTitle();
        const std::string dataFilePath = DataSourceMenu::selectDataSource();
        if (dataFilePath.empty())
        {
            Display::printMessage("No dataset is available. Exiting program.");
            return;
        }

        program.setDataFilePath(dataFilePath);
        program.loadInitialData();
        Display::printMessage("Loaded " + std::to_string(program.list().size()) + " target(s).");
        StructureMenu::run(dataFilePath);
    }
}
