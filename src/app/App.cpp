/*
 * File: App.cpp
 * Description: Implements the application entry workflow.
 */

#include "app/App.h"
#include "core/TargetProgram.h"
#include "ui/MainMenu.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Provide the application entry point used by main.cpp.
     * Design: Keeps process startup separate from core data structure and UI implementations.
     * Workflow: Construct TargetProgram, start MainMenu with it, and return success.
     * Data Handoff: Transfers process control from main.cpp into UI navigation and core program state.
     */
    int runApp()
    {
        TargetProgram program;
        MainMenu::run(program);
        return 0;
    }
}
