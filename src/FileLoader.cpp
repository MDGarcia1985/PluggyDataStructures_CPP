/*
 * File: FileLoader.cpp
 * Description: Implements website data parsing, trimming, and fallback seed entries.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Website: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "FileLoader.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Load website records from a text file into a WebsiteList.
     * Design: Uses a simple pipe-delimited format so data can be edited by hand.
     * Workflow: Open the file, skip comments or malformed lines, trim fields, and append valid websites.
     * Data Handoff: Receives a file path and mutable WebsiteList, then returns true when at least one row loads.
     */
    bool FileLoader::loadWebsitesFromFile(const std::string& filePath, WebsiteList& websites)
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

            std::string name = trim(line.substr(0, separatorIndex));
            std::string url = trim(line.substr(separatorIndex + 1));

            if (!name.empty() && !url.empty())
            {
                websites.addBack(Website(name, url));
                ++loadedCount;
            }
        }

        return loadedCount > 0;
    }

    /*
     * Purpose: Provide starter website data when the file is missing or empty.
     * Design: Keeps fallback values in code so the app can still run without external data.
     * Workflow: Append each built-in Website to the provided list.
     * Data Handoff: Receives a mutable WebsiteList and fills it with default Website objects.
     */
    void FileLoader::loadFallbackWebsites(WebsiteList& websites)
    {
        websites.addBack(Website("Google", "https://www.google.com"));
        websites.addBack(Website("YouTube", "https://www.youtube.com"));
        websites.addBack(Website("Wikipedia", "https://www.wikipedia.org"));
        websites.addBack(Website("GitHub", "https://www.github.com"));
        websites.addBack(Website("Stack Overflow", "https://stackoverflow.com"));
        websites.addBack(Website("Reddit", "https://www.reddit.com"));
        websites.addBack(Website("Amazon", "https://www.amazon.com"));
        websites.addBack(Website("Apple", "https://www.apple.com"));
        websites.addBack(Website("Microsoft", "https://www.microsoft.com"));
        websites.addBack(Website("OpenAI", "https://www.openai.com"));
        websites.addBack(Website("LinkedIn", "https://www.linkedin.com"));
        websites.addBack(Website("Netflix", "https://www.netflix.com"));
        websites.addBack(Website("CNN", "https://www.cnn.com"));
        websites.addBack(Website("BBC", "https://www.bbc.com"));
        websites.addBack(Website("ESPN", "https://www.espn.com"));
        websites.addBack(Website("Khan Academy", "https://www.khanacademy.org"));
        websites.addBack(Website("Coursera", "https://www.coursera.org"));
        websites.addBack(Website("Mozilla", "https://www.mozilla.org"));
        websites.addBack(Website("W3Schools", "https://www.w3schools.com"));
        websites.addBack(Website("GeeksforGeeks", "https://www.geeksforgeeks.org"));
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
