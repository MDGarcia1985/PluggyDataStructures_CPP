/*
 * File: MapSession.cpp
 * Description: Implements interactive word-frequency operations over a TargetMap.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "session/MapSession.h"
#include "ui/Display.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Build a map session with a snapshot of the selected dataset.
     * Design: Keeps source records beside an initially empty analysis map.
     * Workflow: Copy the supplied targets into session-owned storage.
     * Data Handoff: Receives reusable Target values for later field analysis.
     */
    MapSession::MapSession(const std::vector<Target>& items)
        : map_(), loadedTargets_(items)
    {
    }

    /*
     * Purpose: Count words from a full line entered by the user.
     * Design: Keeps console input in the session and delegates parsing and storage to TargetMap.
     * Workflow: Prompt, read a line, reject blank input, replace counts, and report totals.
     * Data Handoff: Moves user text into map_ and displays resulting summary metrics.
     */
    void MapSession::countWordsFromUser()
    {
        std::string text;
        std::cout << "\nEnter text to analyze: ";
        std::getline(std::cin, text);

        if (text.find_first_not_of(" \t\r\n") == std::string::npos)
        {
            Display::printMessage("Text to analyze cannot be blank.");
            return;
        }

        map_.countText(text);
        Display::printMessage(
            "Replaced word counts: " + std::to_string(map_.totalWords()) +
            " total word(s), " + std::to_string(map_.uniqueWords()) + " unique word(s).");
    }

    /*
     * Purpose: Count words from both fields of the selected dataset.
     * Design: Delegates all normalization and map updates while retaining input ownership here.
     * Workflow: Guard an empty snapshot, replace counts from target fields, and report totals.
     * Data Handoff: Sends loadedTargets_ into map_ and displays resulting summary metrics.
     */
    void MapSession::countWordsFromLoadedTargets()
    {
        if (loadedTargets_.empty())
        {
            Display::printMessage("No loaded target data is available.");
            return;
        }

        map_.countTargetFields(loadedTargets_);
        Display::printMessage(
            "Replaced word counts: " + std::to_string(map_.totalWords()) +
            " total word(s), " + std::to_string(map_.uniqueWords()) + " unique word(s).");
    }

    /*
     * Purpose: Display every normalized word and its frequency alphabetically.
     * Design: Relies on std::map iteration order instead of sorting a second collection.
     * Workflow: Guard empty state, print a heading, then print each key and count.
     * Data Handoff: Reads map_.frequencies() and writes a simple table to std::cout.
     */
    void MapSession::showWordFrequencies() const
    {
        if (map_.isEmpty())
        {
            Display::printMessage("No word counts are available yet.");
            return;
        }

        std::cout << "\nWord Frequencies:\n";
        for (const auto& entry : map_.frequencies())
        {
            std::cout << entry.first << ": " << entry.second << '\n';
        }
    }

    /*
     * Purpose: Display all words tied for the greatest occurrence count.
     * Design: Lets TargetMap determine ties while the session owns presentation.
     * Workflow: Guard empty state, print each winning word, then print the shared count.
     * Data Handoff: Reads most-frequent words and maximum frequency and writes them to std::cout.
     */
    void MapSession::showMostFrequentWords() const
    {
        if (map_.isEmpty())
        {
            Display::printMessage("No word counts are available yet.");
            return;
        }

        std::cout << "\nMost frequent word(s):\n";
        for (const std::string& word : map_.mostFrequentWords())
        {
            std::cout << word << '\n';
        }
        std::cout << "Count: " << map_.maxFrequency() << '\n';
    }

    /*
     * Purpose: Display aggregate statistics for the current analysis.
     * Design: Presents metrics already owned or computed by TargetMap.
     * Workflow: Guard empty state and print total, unique, and maximum counts.
     * Data Handoff: Reads three map metrics and writes them to std::cout.
     */
    void MapSession::showSummary() const
    {
        if (map_.isEmpty())
        {
            Display::printMessage("No word counts are available yet.");
            return;
        }

        std::cout << "\nWord Count Summary:\n"
            << "Total words: " << map_.totalWords() << '\n'
            << "Unique words: " << map_.uniqueWords() << '\n'
            << "Maximum frequency: " << map_.maxFrequency() << '\n';
    }

    /*
     * Purpose: Clear the current word-frequency analysis.
     * Design: Delegates state reset to TargetMap and keeps user feedback in the session.
     * Workflow: Clear map_ and print a confirmation message.
     * Data Handoff: Discards map state and reports completion through Display.
     */
    void MapSession::clear()
    {
        map_.clear();
        Display::printMessage("Word counts cleared.");
    }

    /*
     * Purpose: Provide mutable access to the backing word map.
     * Design: Exposes the core object for tests and advanced operations.
     * Workflow: Return map_ by reference.
     * Data Handoff: Gives callers direct access to the session's TargetMap.
     */
    TargetMap& MapSession::map()
    {
        return map_;
    }

    /*
     * Purpose: Provide read-only access to the backing word map.
     * Design: Supports const sessions without allowing mutation.
     * Workflow: Return map_ by const reference.
     * Data Handoff: Gives callers read access to the session's TargetMap.
     */
    const TargetMap& MapSession::map() const
    {
        return map_;
    }
}
