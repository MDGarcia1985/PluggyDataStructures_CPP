/*
 * File: TargetProgram.cpp
 * Description: Implements core business operations for the active target list.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetProgram.h"
#include "io/FileLoader.h"
#include "ui/Display.h"
#include "ui/Menu.h"


// named container pds = Pluggy Data Structures
namespace pds
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
     * Purpose: Change the dataset path used by the next load operation.
     * Design: Keeps file path state private while allowing MainMenu to provide a user selection.
     * Workflow: Move the supplied path into dataFilePath_.
     * Data Handoff: Receives a path from DataSourceMenu through MainMenu for FileLoader use.
     */
    void TargetProgram::setDataFilePath(std::string dataFilePath)
    {
        dataFilePath_ = std::move(dataFilePath);
    }

    /*
     * Purpose: Populate the target list before the menu loop starts.
     * Design: Tries selected data first and falls back to built-in data when loading fails.
     * Workflow: Clear any existing list, load from the data file, then load fallback targets if needed.
     * Data Handoff: Sends dataFilePath_ and targets_ into FileLoader.
     */
    void TargetProgram::loadInitialData()
    {
        FileLoader::loadTargetsOrFallback(dataFilePath_, targets_);
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
        const std::size_t selectedIndex = Menu::promptSelection(targets_.size());
        targets_.removeAt(selectedIndex + 1);

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

}
