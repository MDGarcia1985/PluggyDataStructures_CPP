/*
 * File: Menu.cpp
 * Description: Implements command registration, sorted menu display, and numeric input prompts.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "Menu.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Access the one shared command registry.
     * Design: Uses a function-local static object so the registry is created once on first use.
     * Workflow: Return the same registry reference every time this function is called.
     * Data Handoff: Gives command registration and lookup code a shared storage location.
     */
    CommandRegistry& CommandRegistry::instance()
    {
        static CommandRegistry registry;
        return registry;
    }

    /*
     * Purpose: Add a command plugin to the menu system.
     * Design: Rejects invalid IDs, empty labels, missing actions, and duplicate IDs.
     * Workflow: Validate the command, scan existing commands for duplicates, then store it.
     * Data Handoff: Receives a CommandPlugin and returns whether it was accepted.
     */
    bool CommandRegistry::registerCommand(CommandPlugin command)
    {
        if (command.id < 1 || command.label.empty() || !command.action)
        {
            return false;
        }

        for (const CommandPlugin& existingCommand : commands_)
        {
            if (existingCommand.id == command.id)
            {
                return false;
            }
        }

        commands_.push_back(std::move(command));
        return true;
    }

    /*
     * Purpose: Return the registered commands in menu order.
     * Design: Sorts a copy so callers cannot accidentally reorder the registry's stored commands.
     * Workflow: Copy commands_, sort by command ID, and return the sorted vector.
     * Data Handoff: Gives Menu::display a clean std::vector<CommandPlugin> to print.
     */
    std::vector<CommandPlugin> CommandRegistry::commands() const
    {
        std::vector<CommandPlugin> sortedCommands = commands_;

        std::sort(sortedCommands.begin(), sortedCommands.end(),
            [](const CommandPlugin& left, const CommandPlugin& right)
            {
                return left.id < right.id;
            });

        return sortedCommands;
    }

    /*
     * Purpose: Find a registered command by its numeric menu ID.
     * Design: Returns a pointer so nullptr can signal "not found" without throwing.
     * Workflow: Scan the stored commands and return the matching command address.
     * Data Handoff: Gives TargetProgram either a runnable command or nullptr for invalid input.
     */
    const CommandPlugin* CommandRegistry::findById(int id) const
    {
        for (const CommandPlugin& command : commands_)
        {
            if (command.id == id)
            {
                return &command;
            }
        }

        return nullptr;
    }

    /*
     * Purpose: Print the main menu choices.
     * Design: Receives commands as data so the display code does not need to know who registered them.
     * Workflow: Print a heading, then print each command's ID and label.
     * Data Handoff: Consumes a vector of CommandPlugin values supplied by the registry.
     */
    void Menu::display(const std::vector<CommandPlugin>& commands)
    {
        std::cout << "\n\n\t*** Target Data Structure Menu ***\n\n";

        for (const CommandPlugin& command : commands)
        {
            std::cout << command.id << ") " << command.label << '\n';
        }
    }

    /*
     * Purpose: Ask the user for a main menu choice.
     * Design: Delegates numeric validation to promptInteger() to avoid duplicate input logic.
     * Workflow: Call promptInteger() with the standard menu prompt.
     * Data Handoff: Returns the chosen integer to TargetProgram.
     */
    int Menu::promptChoice()
    {
        return promptInteger("Enter your choice: ");
    }

    /*
     * Purpose: Read a valid integer from the console.
     * Design: Loops until extraction succeeds so non-numeric input does not break the program flow.
     * Workflow: Print the prompt, try to read an int, clear bad input when needed, then discard the newline.
     * Data Handoff: Receives prompt text and returns a validated integer to the caller.
     */
    int Menu::promptInteger(const std::string& prompt)
    {
        int choice = 0;
        std::cout << prompt;

        while (!(std::cin >> choice))
        {
            std::cout << "Invalid input. Please enter a number: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return choice;
    }

    /*
     * Purpose: Ask the user which sample data file should seed the TargetList.
     * Design: Keeps source selection in Menu so TargetProgram can coordinate without owning input details.
     * Workflow: Print the available files, read a numeric choice, and return the matching data path.
     * Data Handoff: Returns a data/<selected_file> path that TargetProgram passes to FileLoader.
     */
    std::string Menu::selectDataSource()
    {
        std::cout << "\nSelect data source:\n";
        std::cout << "1) data/websites.txt\n";
        std::cout << "2) data/messages.txt\n";

        const int choice = promptInteger("Enter your choice: ");

        if (choice == 2)
        {
            return std::string(DATA_DIRECTORY) + "messages.txt";
        }

        return std::string(DATA_DIRECTORY) + "websites.txt";
    }
}
