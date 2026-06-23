/*
 * File: MapSession.cpp
 * Description: Implements interactive word-frequency operations over a TargetMap.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "session/MapSession.h"

#include "algorithms/maps/FrequencyAnalysis.h"
#include "algorithms/maps/MapRanking.h"
#include "algorithms/maps/MapSearch.h"
#include "ui/Display.h"

#include <limits>


// named container pds = Pluggy Data Structures
namespace pds
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
     * Purpose: Display the top N most frequent words.
     * Design: Delegates ranking to FrequencyAnalysis while the session owns prompts and output.
     * Workflow: Guard empty state, read N, compute topN, and print each word/count pair.
     * Data Handoff: Reads map_ and writes ranked results to std::cout.
     */
    void MapSession::showTopNFromUser() const
    {
        if (map_.isEmpty())
        {
            Display::printMessage("No word counts are available yet.");
            return;
        }

        std::cout << "\nHow many top words? ";
        std::size_t count = 0;
        std::cin >> count;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (count == 0)
        {
            Display::printMessage("Count must be greater than zero.");
            return;
        }

        std::cout << "\nTop " << count << " word(s):\n";
        for (const auto& entry : pds::topN(map_, count))
        {
            std::cout << entry.first << ": " << entry.second << '\n';
        }
    }

    /*
     * Purpose: Display the N least frequent words.
     * Design: Delegates ranking to FrequencyAnalysis while the session owns prompts and output.
     * Workflow: Guard empty state, read N, compute leastFrequent, and print each pair.
     * Data Handoff: Reads map_ and writes ranked results to std::cout.
     */
    void MapSession::showLeastFrequentFromUser() const
    {
        if (map_.isEmpty())
        {
            Display::printMessage("No word counts are available yet.");
            return;
        }

        std::cout << "\nHow many least frequent words? ";
        std::size_t count = 0;
        std::cin >> count;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (count == 0)
        {
            Display::printMessage("Count must be greater than zero.");
            return;
        }

        std::cout << "\nLeast frequent " << count << " word(s):\n";
        for (const auto& entry : pds::leastFrequent(map_, count))
        {
            std::cout << entry.first << ": " << entry.second << '\n';
        }
    }

    /*
     * Purpose: Display words grouped by their shared frequency count.
     * Design: Delegates bucket construction to FrequencyAnalysis.
     * Workflow: Guard empty state, build buckets, and print each count with its words.
     * Data Handoff: Reads map_ and writes grouped analysis to std::cout.
     */
    void MapSession::showFrequencyBuckets() const
    {
        if (map_.isEmpty())
        {
            Display::printMessage("No word counts are available yet.");
            return;
        }

        std::cout << "\nFrequency buckets:\n";
        for (const auto& bucket : pds::frequencyBuckets(map_))
        {
            std::cout << "Count " << bucket.first << ':';
            for (const std::string& word : bucket.second)
            {
                std::cout << ' ' << word;
            }
            std::cout << '\n';
        }
    }

    /*
     * Purpose: Search for one word and display its frequency.
     * Design: Delegates lookup to MapSearch while the session owns input and output.
     * Workflow: Prompt for a word, check containsWord, and print the count or not-found message.
     * Data Handoff: Moves user text into MapSearch and displays the result.
     */
    void MapSession::searchWordFromUser() const
    {
        if (map_.isEmpty())
        {
            Display::printMessage("No word counts are available yet.");
            return;
        }

        std::string word;
        std::cout << "\nEnter word to search: ";
        std::getline(std::cin, word);

        if (word.find_first_not_of(" \t\r\n") == std::string::npos)
        {
            Display::printMessage("Search word cannot be blank.");
            return;
        }

        if (pds::containsWord(map_, word))
        {
            Display::printMessage(word + " appears " + std::to_string(map_.countFor(word)) + " time(s).");
        }
        else
        {
            Display::printMessage("Word not found.");
        }
    }

    /*
     * Purpose: List every stored word that starts with a user-supplied prefix.
     * Design: Delegates prefix matching to MapSearch.
     * Workflow: Prompt for a prefix, collect matches, and print them alphabetically.
     * Data Handoff: Moves user text into MapSearch and displays matching keys.
     */
    void MapSession::showWordsStartingWithFromUser() const
    {
        if (map_.isEmpty())
        {
            Display::printMessage("No word counts are available yet.");
            return;
        }

        std::string prefix;
        std::cout << "\nEnter prefix: ";
        std::getline(std::cin, prefix);

        if (prefix.find_first_not_of(" \t\r\n") == std::string::npos)
        {
            Display::printMessage("Prefix cannot be blank.");
            return;
        }

        const std::vector<std::string> matches = pds::wordsStartingWith(map_, prefix);
        if (matches.empty())
        {
            Display::printMessage("No words match that prefix.");
            return;
        }

        std::cout << "\nWords starting with \"" << prefix << "\":\n";
        for (const std::string& word : matches)
        {
            std::cout << word << '\n';
        }
    }

    /*
     * Purpose: Display every word sorted by descending frequency.
     * Design: Delegates ordering to MapRanking.
     * Workflow: Guard empty state, sort by frequency, and print each pair.
     * Data Handoff: Reads map_ and writes ranked results to std::cout.
     */
    void MapSession::showFrequencyRanking() const
    {
        if (map_.isEmpty())
        {
            Display::printMessage("No word counts are available yet.");
            return;
        }

        std::cout << "\nFrequency ranking:\n";
        for (const auto& entry : pds::sortByFrequency(map_))
        {
            std::cout << entry.first << ": " << entry.second << '\n';
        }
    }

    /*
     * Purpose: Display every word in alphabetical order with counts.
     * Design: Delegates ordering to MapRanking.
     * Workflow: Guard empty state, copy alphabetical order, and print each pair.
     * Data Handoff: Reads map_ and writes ranked results to std::cout.
     */
    void MapSession::showAlphabeticalRanking() const
    {
        if (map_.isEmpty())
        {
            Display::printMessage("No word counts are available yet.");
            return;
        }

        std::cout << "\nAlphabetical ranking:\n";
        for (const auto& entry : pds::sortAlphabetically(map_))
        {
            std::cout << entry.first << ": " << entry.second << '\n';
        }
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
