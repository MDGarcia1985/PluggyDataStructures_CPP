/*
 * File: MainMenu.cpp
 * Description: Implements the top-level application menu flow.
 */

#include "core/TargetProgram.h"
#include "ui/DataSourceMenu.h"
#include "ui/Display.h"
#include "ui/MainMenu.h"
#include "ui/TargetDataStructureMenu.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Start dataset selection and hand control to the target command menu.
     * Design: Coordinates menu controllers while leaving loading mechanics in TargetProgram.
     * Workflow: Print the title, choose a dataset, load it, report its size, and run target commands.
     * Data Handoff: Moves the selected path into TargetProgram and passes the program to the next menu.
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
        TargetDataStructureMenu::run(program);
    }
}
