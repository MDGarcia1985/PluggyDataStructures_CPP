/*
 * File: Display.cpp
 * Description: Implements console rendering for titles, lists, current items, and search results.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "ui/Display.h"


// named container llb = linked list browser
namespace llb
{
    namespace Display
    {
        /*
         * Purpose: Print the application title banner.
         * Design: Keeps title formatting in one place instead of scattering cout calls through the controller.
         * Workflow: Write separator lines and the APP_NAME constant to the console.
         * Data Handoff: Reads APP_NAME from shared constants and sends formatted text to std::cout.
         */
        void printTitle()
        {
            std::cout << "\n====================================\n";
            std::cout << "        " << APP_NAME << '\n';
            std::cout << "====================================\n";
        }

        /*
         * Purpose: Print a standalone status or error message.
         * Design: Wraps messages with blank lines so user feedback is easy to see.
         * Workflow: Write the message between newline characters.
         * Data Handoff: Receives text from controller code and sends it to std::cout.
         */
        void printMessage(const std::string& message)
        {
            std::cout << '\n' << message << '\n';
        }

        /*
         * Purpose: Print one Target, optionally with a numbered list position.
         * Design: Uses Target::toDisplayString() so formatting rules stay with the Target object.
         * Workflow: Print the position when provided, then print the formatted Target text.
         * Data Handoff: Receives a Target and optional position, then writes display text to std::cout.
         */
        void printTarget(const Target& target, std::size_t position)
        {
            if (position > 0)
            {
                std::cout << position << ") ";
            }

            std::cout << target.toDisplayString() << '\n';
        }

        /*
         * Purpose: Print every Target in a list.
         * Design: Works from a vector snapshot so Display does not need access to linked-list nodes.
         * Workflow: Handle the empty case, print a heading, then print each Target with a one-based number.
         * Data Handoff: Receives std::vector<Target> from TargetList::toVector().
         */
        void printTargetList(const std::vector<Target>& targets)
        {
            if (targets.empty())
            {
                printMessage("The target list is empty.");
                return;
            }

            std::cout << "\nTarget List:\n";

            for (std::size_t i = 0; i < targets.size(); ++i)
            {
                printTarget(targets[i], i + 1);
            }
        }

        /*
         * Purpose: Print the currently selected Target.
         * Design: Accepts a pointer so nullptr can cleanly represent "no current Target."
         * Workflow: Show an empty-state message for nullptr, otherwise print the position and Target.
         * Data Handoff: Receives current Target data and position from TargetList.
         */
        void printCurrentTarget(const Target* target, std::size_t position)
        {
            if (target == nullptr)
            {
                printMessage("No current record to display.");
                return;
            }

            std::cout << "\nCurrent record";
            if (position > 0)
            {
                std::cout << " [" << position << "]";
            }
            std::cout << ":\n";

            printTarget(*target);
        }

        /*
         * Purpose: Print all search matches.
         * Design: Uses TargetSearchResult so each displayed item keeps its original list position.
         * Workflow: Handle no matches, print a heading, then print each result's Target and position.
         * Data Handoff: Receives search results from TargetList::findAll().
         */
        void printSearchResults(const std::vector<TargetSearchResult>& results)
        {
            if (results.empty())
            {
                printMessage("No matching target found.");
                return;
            }

            std::cout << "\nSearch Results:\n";

            for (const TargetSearchResult& result : results)
            {
                printTarget(result.target, result.position);
            }
        }
    }
}

