/*
 * File: WebsiteProgram.cpp
 * Description: Implements built-in commands and the main controller loop for the browser.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Website: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "App.h"


// named container llb = linked list browser
namespace llb
{
    namespace
    {
        /*
         * Purpose: Adapt the display-list menu command to the WebsiteProgram method.
         * Design: Keeps command registration simple by using a free function with the expected signature.
         * Workflow: Receive the active program and call displayList().
         * Data Handoff: Passes control from the command registry into the controller object.
         */
        void displayListCommand(WebsiteProgram& program)
        {
            program.displayList();
        }

        /*
         * Purpose: Adapt the forward-navigation menu command to the WebsiteProgram method.
         * Design: Uses the same command function signature as every other menu action.
         * Workflow: Receive the active program and call goForward().
         * Data Handoff: Passes user menu intent into the controller object.
         */
        void goForwardCommand(WebsiteProgram& program)
        {
            program.goForward();
        }

        /*
         * Purpose: Adapt the backward-navigation menu command to the WebsiteProgram method.
         * Design: Uses a tiny wrapper so command registration does not expose class internals.
         * Workflow: Receive the active program and call goBackward().
         * Data Handoff: Passes user menu intent into the controller object.
         */
        void goBackwardCommand(WebsiteProgram& program)
        {
            program.goBackward();
        }

        /*
         * Purpose: Adapt the add-website menu command to the WebsiteProgram method.
         * Design: Keeps the command registry independent from member-function pointer syntax.
         * Workflow: Receive the active program and call addWebsiteFromUser().
         * Data Handoff: Passes user menu intent into the controller object.
         */
        void addWebsiteCommand(WebsiteProgram& program)
        {
            program.addWebsiteFromUser();
        }

        /*
         * Purpose: Adapt the delete-website menu command to the WebsiteProgram method.
         * Design: Uses the shared command signature required by CommandPlugin.
         * Workflow: Receive the active program and call deleteWebsiteFromUser().
         * Data Handoff: Passes user menu intent into the controller object.
         */
        void deleteWebsiteCommand(WebsiteProgram& program)
        {
            program.deleteWebsiteFromUser();
        }

        /*
         * Purpose: Adapt the find-website menu command to the WebsiteProgram method.
         * Design: Keeps search behavior inside WebsiteProgram while exposing it as a menu command.
         * Workflow: Receive the active program and call findWebsiteFromUser().
         * Data Handoff: Passes user menu intent into the controller object.
         */
        void findWebsiteCommand(WebsiteProgram& program)
        {
            program.findWebsiteFromUser();
        }

        /*
         * Purpose: Handle the exit menu command.
         * Design: Requests exit through WebsiteProgram instead of directly breaking the main loop here.
         * Workflow: Print an exit message, then set the program's exit flag.
         * Data Handoff: Sends user intent into WebsiteProgram::requestExit().
         */
        void exitCommand(WebsiteProgram& program)
        {
            Display::printMessage("Exiting program.");
            program.requestExit();
        }
    }
}

LLB_REGISTER_COMMAND(1, "Display the list", llb::displayListCommand)
LLB_REGISTER_COMMAND(2, "Go forward and display the webpage", llb::goForwardCommand)
LLB_REGISTER_COMMAND(3, "Go backward and display the webpage", llb::goBackwardCommand)
LLB_REGISTER_COMMAND(4, "Add another item to the list", llb::addWebsiteCommand)
LLB_REGISTER_COMMAND(5, "Delete an item from the list", llb::deleteWebsiteCommand)
LLB_REGISTER_COMMAND(6, "Find an item in the list", llb::findWebsiteCommand)
LLB_REGISTER_COMMAND(7, "Exit", llb::exitCommand)

namespace llb
{
    /*
     * Purpose: Build the application controller.
     * Design: Stores the website list, exit flag, and data file path together in one controller object.
     * Workflow: Start with an empty list, no exit request, and the provided data file path.
     * Data Handoff: Receives the file path used later by loadInitialData().
     */
    WebsiteProgram::WebsiteProgram(std::string dataFilePath)
        : websites_(), exitRequested_(false), dataFilePath_(std::move(dataFilePath))
    {
    }

    /*
     * Purpose: Run the interactive website browser.
     * Design: Uses registered commands so menu behavior can expand without hard-coding every action here.
     * Workflow: Print the title, load data, loop until exit, display commands, read a choice, and run the command.
     * Data Handoff: Moves user input from Menu into CommandRegistry, then into command actions.
     */
    void WebsiteProgram::run()
    {
        Display::printTitle();
        loadInitialData();
        Display::printMessage("Loaded " + std::to_string(websites_.size()) + " website(s).");

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
     * Purpose: Populate the website list before the menu loop starts.
     * Design: Tries external data first and falls back to built-in data when loading fails.
     * Workflow: Clear any existing list, load from the data file, then load fallback websites if needed.
     * Data Handoff: Sends dataFilePath_ and websites_ into FileLoader.
     */
    void WebsiteProgram::loadInitialData()
    {
        websites_.clear();

        if (!FileLoader::loadWebsitesFromFile(dataFilePath_, websites_))
        {
            FileLoader::loadFallbackWebsites(websites_);
        }
    }

    /*
     * Purpose: Tell the run loop to stop after the current command finishes.
     * Design: Uses a boolean flag so command functions do not need to control loop mechanics directly.
     * Workflow: Set exitRequested_ to true.
     * Data Handoff: Receives exit intent from the exit command.
     */
    void WebsiteProgram::requestExit()
    {
        exitRequested_ = true;
    }

    /*
     * Purpose: Report whether the user has requested program exit.
     * Design: Keeps the exit flag private while allowing read-only checks.
     * Workflow: Return the current value of exitRequested_.
     * Data Handoff: Gives the run loop and tests the current exit state.
     */
    bool WebsiteProgram::exitRequested() const
    {
        return exitRequested_;
    }

    /*
     * Purpose: Provide mutable access to the website list.
     * Design: Exposes the list for tests and internal workflows that need to modify it.
     * Workflow: Return websites_ by reference.
     * Data Handoff: Gives callers direct access to the controller's WebsiteList.
     */
    WebsiteList& WebsiteProgram::list()
    {
        return websites_;
    }

    /*
     * Purpose: Provide read-only access to the website list.
     * Design: Supports const WebsiteProgram objects without allowing list mutation.
     * Workflow: Return websites_ by const reference.
     * Data Handoff: Gives callers read access to the controller's WebsiteList.
     */
    const WebsiteList& WebsiteProgram::list() const
    {
        return websites_;
    }

    /*
     * Purpose: Display all websites currently stored in the list.
     * Design: Converts the linked list to a vector so Display does not depend on node internals.
     * Workflow: Ask WebsiteList for a vector snapshot, then pass it to Display.
     * Data Handoff: Moves website data from WebsiteList to Display as std::vector<Website>.
     */
    void WebsiteProgram::displayList() const
    {
        Display::printWebsiteList(websites_.toVector());
    }

    /*
     * Purpose: Display the currently selected website.
     * Design: Keeps current pointer logic inside WebsiteList and output formatting inside Display.
     * Workflow: Ask the list for current Website and position, then pass both to Display.
     * Data Handoff: Moves current selection data from WebsiteList to Display.
     */
    void WebsiteProgram::displayCurrent() const
    {
        Display::printCurrentWebsite(websites_.current(), websites_.currentPosition());
    }

    /*
     * Purpose: Move forward one website and show the result.
     * Design: Lets WebsiteList handle wraparound navigation and empty-list detection.
     * Workflow: Try to move forward, print an empty message if it fails, otherwise display current.
     * Data Handoff: Receives a success flag from WebsiteList and sends display data to Display.
     */
    void WebsiteProgram::goForward()
    {
        if (!websites_.moveForward())
        {
            Display::printMessage("The website list is empty.");
            return;
        }

        displayCurrent();
    }

    /*
     * Purpose: Move backward one website and show the result.
     * Design: Lets WebsiteList handle wraparound navigation and empty-list detection.
     * Workflow: Try to move backward, print an empty message if it fails, otherwise display current.
     * Data Handoff: Receives a success flag from WebsiteList and sends display data to Display.
     */
    void WebsiteProgram::goBackward()
    {
        if (!websites_.moveBackward())
        {
            Display::printMessage("The website list is empty.");
            return;
        }

        displayCurrent();
    }

    /*
     * Purpose: Add a new website from user input.
     * Design: Keeps input collection in the controller and storage mechanics in WebsiteList.
     * Workflow: Prompt for name and URL, reject blanks, create a Website, and append it.
     * Data Handoff: Moves console input into a Website object, then into websites_.
     */
    void WebsiteProgram::addWebsiteFromUser()
    {
        std::string name;
        std::string url;

        std::cout << "\nEnter website name: ";
        std::getline(std::cin, name);

        std::cout << "Enter website URL: ";
        std::getline(std::cin, url);

        if (name.empty() || url.empty())
        {
            Display::printMessage("Website name and URL cannot be empty.");
            return;
        }

        websites_.addBack(Website(name, url));
        Display::printMessage("Website added successfully.");
    }

    /*
     * Purpose: Delete a website selected by the user.
     * Design: Shows numbered list positions so the user can choose by the same one-based index WebsiteList expects.
     * Workflow: Check for empty list, display the list, prompt for a position, then remove that position.
     * Data Handoff: Moves a numeric choice from Menu into WebsiteList::removeAt().
     */
    void WebsiteProgram::deleteWebsiteFromUser()
    {
        if (websites_.isEmpty())
        {
            Display::printMessage("The website list is empty.");
            return;
        }

        displayList();
        const int position = Menu::promptInteger("\nEnter the number of the website to delete: ");

        if (position < 1 || !websites_.removeAt(static_cast<std::size_t>(position)))
        {
            Display::printMessage("Invalid position. No website was deleted.");
            return;
        }

        Display::printMessage("Website deleted successfully.");
    }

    /*
     * Purpose: Search websites by name or URL and select the first match.
     * Design: WebsiteList performs matching while Display handles result formatting.
     * Workflow: Prompt for a search term, reject blanks, display matches, and set current to the first result.
     * Data Handoff: Moves user text into WebsiteList::findAll(), then uses result positions to update current.
     */
    void WebsiteProgram::findWebsiteFromUser()
    {
        if (websites_.isEmpty())
        {
            Display::printMessage("The website list is empty.");
            return;
        }

        std::string searchTerm;
        std::cout << "\nEnter website name or URL to search for: ";
        std::getline(std::cin, searchTerm);

        if (searchTerm.empty())
        {
            Display::printMessage("Search term cannot be empty.");
            return;
        }

        const std::vector<WebsiteSearchResult> results = websites_.findAll(searchTerm);
        Display::printSearchResults(results);

        if (!results.empty())
        {
            websites_.setCurrentToPosition(results.front().position);
            Display::printMessage("Current webpage set to the first matching result.");
        }
    }

    /*
     * Purpose: Provide a simple entry point for main.cpp.
     * Design: Keeps main.cpp tiny by creating and running the controller here.
     * Workflow: Construct WebsiteProgram, run it, and return a process success code.
     * Data Handoff: Hands control from main.cpp to the application controller.
     */
    int runApp()
    {
        WebsiteProgram program;
        program.run();
        return 0;
    }
}
