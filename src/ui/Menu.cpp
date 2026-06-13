/*
 * File: Menu.cpp
 * Description: Implements reusable console menu display and validated input.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "ui/Menu.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Display a titled list of menu option labels.
     * Design: Accepts plain strings so this reusable UI helper has no registry or domain dependencies.
     * Workflow: Print the title and each option with a generated one-based position.
     * Data Handoff: Receives labels from a menu controller and writes them to the console.
     */
    void Menu::display(const std::string& title, const std::vector<std::string>& options)
    {
        std::cout << '\n' << title << "\n\n";

        for (std::size_t index = 0; index < options.size(); ++index)
        {
            std::cout << index + 1 << ") " << options[index] << '\n';
        }
    }

    /*
     * Purpose: Display a complete menu until the user makes a valid selection.
     * Design: Combines generic rendering and range validation without knowing what an option does.
     * Workflow: Render the labels, read a choice, return its zero-based index, or redisplay after errors.
     * Data Handoff: Converts a controller's title and labels into a validated vector index.
     */
    std::size_t Menu::select(const std::string& title, const std::vector<std::string>& options)
    {
        while (true)
        {
            display(title, options);
            const int choice = promptChoice();

            if (choice >= 1 && static_cast<std::size_t>(choice) <= options.size())
            {
                return static_cast<std::size_t>(choice - 1);
            }

            printInvalidSelection();
        }
    }

    /*
     * Purpose: Read a choice that is guaranteed to match a previously displayed option count.
     * Design: Applies range validation in a retry loop for numbered lists rendered by other modules.
     * Workflow: Prompt, accept a one-based in-range value, or print an error and retry.
     * Data Handoff: Returns a zero-based index to the calling controller or business operation.
     */
    std::size_t Menu::promptSelection(std::size_t optionCount)
    {
        while (true)
        {
            const int choice = promptChoice();
            if (choice >= 1 && static_cast<std::size_t>(choice) <= optionCount)
            {
                return static_cast<std::size_t>(choice - 1);
            }

            printInvalidSelection();
        }
    }

    /*
     * Purpose: Ask the user for a numeric menu choice.
     * Design: Delegates stream recovery to the generic integer prompt.
     * Workflow: Call promptInteger() with the standard menu prompt.
     * Data Handoff: Returns an entered integer for range validation.
     */
    int Menu::promptChoice()
    {
        return promptInteger("Enter your choice: ");
    }

    /*
     * Purpose: Read an integer without leaving the console stream in a failed state.
     * Design: Repeats non-numeric input and discards the remainder of each failed line.
     * Workflow: Print the prompt, extract an integer, recover failures, and consume the newline.
     * Data Handoff: Converts console text into an integer for menu and record selection.
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
     * Purpose: Print the standard out-of-range selection message.
     * Design: Centralizes wording so every menu reports invalid choices consistently.
     * Workflow: Write the shared validation message to the console.
     * Data Handoff: Sends menu validation feedback to the user.
     */
    void Menu::printInvalidSelection()
    {
        std::cout << "Invalid selection. Please choose one of the listed options.\n";
    }
}
