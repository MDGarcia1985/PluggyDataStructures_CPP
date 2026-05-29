/*
 * File: Display.cpp
 * Description: Implements console rendering for titles, lists, current items, and search results.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Website: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "Display.h"


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
         * Purpose: Print one website, optionally with a numbered list position.
         * Design: Uses Website::toDisplayString() so formatting rules stay with the Website object.
         * Workflow: Print the position when provided, then print the formatted website text.
         * Data Handoff: Receives a Website and optional position, then writes display text to std::cout.
         */
        void printWebsite(const Website& website, std::size_t position)
        {
            if (position > 0)
            {
                std::cout << position << ") ";
            }

            std::cout << website.toDisplayString() << '\n';
        }

        /*
         * Purpose: Print every website in a list.
         * Design: Works from a vector snapshot so Display does not need access to linked-list nodes.
         * Workflow: Handle the empty case, print a heading, then print each website with a one-based number.
         * Data Handoff: Receives std::vector<Website> from WebsiteList::toVector().
         */
        void printWebsiteList(const std::vector<Website>& websites)
        {
            if (websites.empty())
            {
                printMessage("The website list is empty.");
                return;
            }

            std::cout << "\nWebsite List:\n";

            for (std::size_t i = 0; i < websites.size(); ++i)
            {
                printWebsite(websites[i], i + 1);
            }
        }

        /*
         * Purpose: Print the currently selected website.
         * Design: Accepts a pointer so nullptr can cleanly represent "no current website."
         * Workflow: Show an empty-state message for nullptr, otherwise print the position and website.
         * Data Handoff: Receives current Website data and position from WebsiteList.
         */
        void printCurrentWebsite(const Website* website, std::size_t position)
        {
            if (website == nullptr)
            {
                printMessage("No current webpage to display.");
                return;
            }

            std::cout << "\nCurrent Webpage";
            if (position > 0)
            {
                std::cout << " [" << position << "]";
            }
            std::cout << ":\n";

            printWebsite(*website);
        }

        /*
         * Purpose: Print all search matches.
         * Design: Uses WebsiteSearchResult so each displayed item keeps its original list position.
         * Workflow: Handle no matches, print a heading, then print each result's website and position.
         * Data Handoff: Receives search results from WebsiteList::findAll().
         */
        void printSearchResults(const std::vector<WebsiteSearchResult>& results)
        {
            if (results.empty())
            {
                printMessage("No matching website found.");
                return;
            }

            std::cout << "\nSearch Results:\n";

            for (const WebsiteSearchResult& result : results)
            {
                printWebsite(result.website, result.position);
            }
        }
    }
}
