/*
 * File: TargetProgram.cpp
 * Description: Implements built-in commands and the main controller loop for generic targets.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "Display.h"
#include "FileLoader.h"
#include "Menu.h"
#include "TargetProgram.h"


// named container llb = linked list browser
namespace llb
{
    namespace
    {
        /*
         * Purpose: Adapt the display-list menu command to the TargetProgram method.
         * Design: Keeps command registration simple by using a free function with the expected signature.
         * Workflow: Receive the active program and call displayList().
         * Data Handoff: Passes control from the command registry into the controller object.
         */
        void displayListCommand(TargetProgram& program)
        {
            program.displayList();
        }

        /*
         * Purpose: Adapt the forward-navigation menu command to the TargetProgram method.
         * Design: Uses the same command function signature as every other menu action.
         * Workflow: Receive the active program and call goForward().
         * Data Handoff: Passes user menu intent into the controller object.
         */
        void goForwardCommand(TargetProgram& program)
        {
            program.goForward();
        }

        /*
         * Purpose: Adapt the backward-navigation menu command to the TargetProgram method.
         * Design: Uses a tiny wrapper so command registration does not expose class internals.
         * Workflow: Receive the active program and call goBackward().
         * Data Handoff: Passes user menu intent into the controller object.
         */
        void goBackwardCommand(TargetProgram& program)
        {
            program.goBackward();
        }

        /*
         * Purpose: Adapt the add-target menu command to the TargetProgram method.
         * Design: Keeps the command registry independent from member-function pointer syntax.
         * Workflow: Receive the active program and call addTargetFromUser().
         * Data Handoff: Passes user menu intent into the controller object.
         */
        void addTargetCommand(TargetProgram& program)
        {
            program.addTargetFromUser();
        }

        /*
         * Purpose: Adapt the delete-target menu command to the TargetProgram method.
         * Design: Uses the shared command signature required by CommandPlugin.
         * Workflow: Receive the active program and call deleteTargetFromUser().
         * Data Handoff: Passes user menu intent into the controller object.
         */
        void deleteTargetCommand(TargetProgram& program)
        {
            program.deleteTargetFromUser();
        }

        /*
         * Purpose: Adapt the find-target menu command to the TargetProgram method.
         * Design: Keeps search behavior inside TargetProgram while exposing it as a menu command.
         * Workflow: Receive the active program and call findTargetFromUser().
         * Data Handoff: Passes user menu intent into the controller object.
         */
        void findTargetCommand(TargetProgram& program)
        {
            program.findTargetFromUser();
        }

        /*
         * Purpose: Handle the exit menu command.
         * Design: Requests exit through TargetProgram instead of directly breaking the main loop here.
         * Workflow: Print an exit message, then set the program's exit flag.
         * Data Handoff: Sends user intent into TargetProgram::requestExit().
         */
        void exitCommand(TargetProgram& program)
        {
            Display::printMessage("Exiting program.");
            program.requestExit();
        }
    }
}

LLB_REGISTER_COMMAND(1, "Display the list", llb::displayListCommand)
LLB_REGISTER_COMMAND(2, "Go forward and display the current target", llb::goForwardCommand)
LLB_REGISTER_COMMAND(3, "Go backward and display the current target", llb::goBackwardCommand)
LLB_REGISTER_COMMAND(4, "Add another target to the list", llb::addTargetCommand)
LLB_REGISTER_COMMAND(5, "Delete a target from the list", llb::deleteTargetCommand)
LLB_REGISTER_COMMAND(6, "Find a target in the list", llb::findTargetCommand)
LLB_REGISTER_COMMAND(7, "Exit", llb::exitCommand)

namespace llb
{
    /*
     * Purpose: Build the application controller.
     * Design: Stores the target list, exit flag, and data file path together in one controller object.
     * Workflow: Start with an empty list, no exit request, and the provided data file path.
     * Data Handoff: Receives the file path used later by loadInitialData().
     */
    TargetProgram::TargetProgram(std::string dataFilePath)
        : targets_(), exitRequested_(false), dataFilePath_(std::move(dataFilePath))
    {
    }

    /*
     * Purpose: Run the interactive generic data-structure browser.
     * Design: Uses registered commands so menu behavior can expand without hard-coding every action here.
     * Workflow: Print the title, select data, load it, loop until exit, display commands, read a choice, and run it.
     * Data Handoff: Moves data-source and command input from Menu into TargetProgram and CommandRegistry.
     */
    void TargetProgram::run()
    {
        Display::printTitle();
        dataFilePath_ = Menu::selectDataSource();
        loadInitialData();
        Display::printMessage("Loaded " + std::to_string(targets_.size()) + " target(s).");

        while (!exitRequested_)
        {
            const std::vector<CommandPlugin> commands = CommandRegistry::instance().commands();
            Menu::display(commands);

            const int menuChoice = Menu::promptChoice();
            const CommandPlugin* command = CommandRegistry::instance().findById(menuChoice);

            if (command == nullptr)
            {
                Display::printMessage("Invalid option. Please choose a listed command.");
                continue;
            }

            command->action(*this);
        }
    }

    /*
     * Purpose: Populate the target list before the menu loop starts.
     * Design: Tries selected data first and falls back to built-in data when loading fails.
     * Workflow: Clear any existing list, load from the data file, then load fallback targets if needed.
     * Data Handoff: Sends dataFilePath_ and targets_ into FileLoader.
     */
    void TargetProgram::loadInitialData()
    {
        targets_.clear();

        if (!FileLoader::loadTargetsFromFile(dataFilePath_, targets_))
        {
            FileLoader::loadFallbackTargets(targets_);
        }
    }

    /*
     * Purpose: Tell the run loop to stop after the current command finishes.
     * Design: Uses a boolean flag so command functions do not need to control loop mechanics directly.
     * Workflow: Set exitRequested_ to true.
     * Data Handoff: Receives exit intent from the exit command.
     */
    void TargetProgram::requestExit()
    {
        exitRequested_ = true;
    }

    /*
     * Purpose: Report whether the user has requested program exit.
     * Design: Keeps the exit flag private while allowing read-only checks.
     * Workflow: Return the current value of exitRequested_.
     * Data Handoff: Gives the run loop and tests the current exit state.
     */
    bool TargetProgram::exitRequested() const
    {
        return exitRequested_;
    }

    /*
     * Purpose: Provide mutable access to the target list.
     * Design: Exposes the list for tests and internal workflows that need to modify it.
     * Workflow: Return targets_ by reference.
     * Data Handoff: Gives callers direct access to the controller's TargetList.
     */
    TargetList& TargetProgram::list()
    {
        return targets_;
    }

    /*
     * Purpose: Provide read-only access to the target list.
     * Design: Supports const TargetProgram objects without allowing list mutation.
     * Workflow: Return targets_ by const reference.
     * Data Handoff: Gives callers read access to the controller's TargetList.
     */
    const TargetList& TargetProgram::list() const
    {
        return targets_;
    }

    /*
     * Purpose: Display all targets currently stored in the list.
     * Design: Converts the linked list to a vector so Display does not depend on node internals.
     * Workflow: Ask TargetList for a vector snapshot, then pass it to Display.
     * Data Handoff: Moves target data from TargetList to Display as std::vector<Target>.
     */
    void TargetProgram::displayList() const
    {
        Display::printTargetList(targets_.toVector());
    }

    /*
     * Purpose: Display the currently selected target.
     * Design: Keeps current pointer logic inside TargetList and output formatting inside Display.
     * Workflow: Ask the list for current Target and position, then pass both to Display.
     * Data Handoff: Moves current selection data from TargetList to Display.
     */
    void TargetProgram::displayCurrent() const
    {
        Display::printCurrentTarget(targets_.current(), targets_.currentPosition());
    }

    /*
     * Purpose: Move forward one target and show the result.
     * Design: Lets TargetList handle wraparound navigation and empty-list detection.
     * Workflow: Try to move forward, print an empty message if it fails, otherwise display current.
     * Data Handoff: Receives a success flag from TargetList and sends display data to Display.
     */
    void TargetProgram::goForward()
    {
        if (!targets_.moveForward())
        {
            Display::printMessage("The target list is empty.");
            return;
        }

        displayCurrent();
    }

    /*
     * Purpose: Move backward one target and show the result.
     * Design: Lets TargetList handle wraparound navigation and empty-list detection.
     * Workflow: Try to move backward, print an empty message if it fails, otherwise display current.
     * Data Handoff: Receives a success flag from TargetList and sends display data to Display.
     */
    void TargetProgram::goBackward()
    {
        if (!targets_.moveBackward())
        {
            Display::printMessage("The target list is empty.");
            return;
        }

        displayCurrent();
    }

    /*
     * Purpose: Add a new generic target from user input.
     * Design: Keeps input collection in the controller and storage mechanics in TargetList.
     * Workflow: Prompt for two fields, reject blanks, create a Target, and append it.
     * Data Handoff: Moves console input into a Target object, then into targets_.
     */
    void TargetProgram::addTargetFromUser()
    {
        std::string fieldOne;
        std::string fieldTwo;

        std::cout << "\nEnter first field: ";
        std::getline(std::cin, fieldOne);

        std::cout << "Enter second field: ";
        std::getline(std::cin, fieldTwo);

        if (fieldOne.empty() || fieldTwo.empty())
        {
            Display::printMessage("Both target fields are required.");
            return;
        }

        targets_.addBack(Target(fieldOne, fieldTwo));
        Display::printMessage("Target added successfully.");
    }

    /*
     * Purpose: Delete a target selected by the user.
     * Design: Shows numbered list positions so the user can choose by the same one-based index TargetList expects.
     * Workflow: Check for empty list, display the list, prompt for a position, then remove that position.
     * Data Handoff: Moves a numeric choice from Menu into TargetList::removeAt().
     */
    void TargetProgram::deleteTargetFromUser()
    {
        if (targets_.isEmpty())
        {
            Display::printMessage("The target list is empty.");
            return;
        }

        displayList();
        const int position = Menu::promptInteger("\nEnter the number of the target to delete: ");

        if (position < 1 || !targets_.removeAt(static_cast<std::size_t>(position)))
        {
            Display::printMessage("Invalid position. No target was deleted.");
            return;
        }

        Display::printMessage("Target deleted successfully.");
    }

    /*
     * Purpose: Search targets by either field and select the first match.
     * Design: TargetList performs matching while Display handles result formatting.
     * Workflow: Prompt for a search term, reject blanks, display matches, and set current to the first result.
     * Data Handoff: Moves user text into TargetList::findAll(), then uses result positions to update current.
     */
    void TargetProgram::findTargetFromUser()
    {
        if (targets_.isEmpty())
        {
            Display::printMessage("The target list is empty.");
            return;
        }

        std::string searchTerm;
        std::cout << "\nEnter text to search for: ";
        std::getline(std::cin, searchTerm);

        if (searchTerm.empty())
        {
            Display::printMessage("Search term cannot be empty.");
            return;
        }

        const std::vector<TargetSearchResult> results = targets_.findAll(searchTerm);
        Display::printSearchResults(results);

        if (!results.empty())
        {
            targets_.setCurrentToPosition(results.front().position);
            Display::printMessage("Current target set to the first matching result.");
        }
    }

    /*
     * Purpose: Provide a simple entry point for main.cpp.
     * Design: Keeps main.cpp tiny by creating and running the controller here.
     * Workflow: Construct TargetProgram, run it, and return a process success code.
     * Data Handoff: Hands control from main.cpp to the application controller.
     */
    int runApp()
    {
        TargetProgram program;
        program.run();
        return 0;
    }
}
