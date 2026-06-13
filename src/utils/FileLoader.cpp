/*
 * File: FileLoader.cpp
 * Description: Implements generic target data parsing, trimming, and fallback seed entries.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "FileLoader.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Load generic two-field records from a text file into a TargetList.
     * Design: Uses a simple pipe-delimited format so different sample data can be edited by hand.
     * Workflow: Open the selected file, skip comments or malformed lines, trim fields, and append valid targets.
     * Data Handoff: Receives a file path and mutable TargetList, then returns true when at least one row loads.
     */
    bool FileLoader::loadTargetsFromFile(const std::string& filePath, TargetList& targets)
    {
        std::ifstream inputFile(filePath);

        if (!inputFile.is_open())
        {
            return false;
        }

        std::string line;
        std::size_t loadedCount = 0;

        while (std::getline(inputFile, line))
        {
            line = trim(line);

            if (line.empty() || line[0] == '#')
            {
                continue;
            }

            const std::size_t separatorIndex = line.find('|');
            if (separatorIndex == std::string::npos)
            {
                continue;
            }

            std::string fieldOne = trim(line.substr(0, separatorIndex));
            std::string fieldTwo = trim(line.substr(separatorIndex + 1));

            if (!fieldOne.empty() && !fieldTwo.empty())
            {
                targets.addBack(Target(fieldOne, fieldTwo));
                ++loadedCount;
            }
        }

        return loadedCount > 0;
    }

    /*
     * Purpose: Provide starter target data when the selected file is missing or empty.
     * Design: Keeps fallback values generic so the app can still demonstrate the data structure.
     * Workflow: Append each built-in Target to the provided list.
     * Data Handoff: Receives a mutable TargetList and fills it with default Target objects.
     */
    void FileLoader::loadFallbackTargets(TargetList& targets)
    {
        targets.addBack(Target("Google", "https://www.google.com"));
        targets.addBack(Target("YouTube", "https://www.youtube.com"));
        targets.addBack(Target("Wikipedia", "https://www.wikipedia.org"));
        targets.addBack(Target("GitHub", "https://www.github.com"));
        targets.addBack(Target("Stack Overflow", "https://stackoverflow.com"));
        targets.addBack(Target("Reddit", "https://www.reddit.com"));
        targets.addBack(Target("Amazon", "https://www.amazon.com"));
        targets.addBack(Target("Apple", "https://www.apple.com"));
        targets.addBack(Target("Microsoft", "https://www.microsoft.com"));
        targets.addBack(Target("OpenAI", "https://www.openai.com"));
        targets.addBack(Target("LinkedIn", "https://www.linkedin.com"));
        targets.addBack(Target("Netflix", "https://www.netflix.com"));
        targets.addBack(Target("CNN", "https://www.cnn.com"));
        targets.addBack(Target("BBC", "https://www.bbc.com"));
        targets.addBack(Target("ESPN", "https://www.espn.com"));
        targets.addBack(Target("Khan Academy", "https://www.khanacademy.org"));
        targets.addBack(Target("Coursera", "https://www.coursera.org"));
        targets.addBack(Target("Mozilla", "https://www.mozilla.org"));
        targets.addBack(Target("W3Schools", "https://www.w3schools.com"));
        targets.addBack(Target("GeeksforGeeks", "https://www.geeksforgeeks.org"));
    }

    /*
     * Purpose: Remove leading and trailing whitespace from input text.
     * Design: Treats spaces, tabs, and line endings as removable edges while keeping interior text intact.
     * Workflow: Find the first and last non-whitespace characters and return that substring.
     * Data Handoff: Receives raw text from a file line or field and returns cleaned text for parsing.
     */
    std::string FileLoader::trim(const std::string& text)
    {
        const std::string whitespace = " \t\n\r";
        const std::size_t first = text.find_first_not_of(whitespace);

        if (first == std::string::npos)
        {
            return "";
        }

        const std::size_t last = text.find_last_not_of(whitespace);
        return text.substr(first, last - first + 1);
    }
}
