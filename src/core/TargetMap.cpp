/*
 * File: TargetMap.cpp
 * Description: Implements normalized word counting with alphabetically ordered map storage.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetMap.h"

#include <cctype>


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Create an empty word-frequency map.
     * Design: Initializes the explicit total independently from the unique-key map.
     * Workflow: Construct empty storage and set the total count to zero.
     * Data Handoff: Produces a ready-to-use TargetMap with no counted words.
     */
    TargetMap::TargetMap()
        : wordCounts_(), totalWords_(0)
    {
    }

    /*
     * Purpose: Convert one parsed word to its canonical lowercase representation.
     * Design: Keeps only alphanumeric bytes and applies ctype operations through unsigned char.
     * Workflow: Scan the input, retain alphanumeric characters, and lowercase each retained byte.
     * Data Handoff: Returns normalized text to the counting helper.
     */
    std::string TargetMap::normalizeWord(const std::string& word)
    {
        std::string normalized;
        normalized.reserve(word.size());

        for (const char character : word)
        {
            const unsigned char value = static_cast<unsigned char>(character);
            if (std::isalnum(value))
            {
                normalized.push_back(static_cast<char>(std::tolower(value)));
            }
        }

        return normalized;
    }

    /*
     * Purpose: Add one raw token to a frequency map when normalization yields a word.
     * Design: Centralizes count and total updates so all input sources behave identically.
     * Workflow: Normalize the token, ignore an empty result, then increment its map and total counts.
     * Data Handoff: Mutates the supplied count map and total through references.
     */
    void TargetMap::addNormalizedWord(
        std::map<std::string, int>& counts,
        int& totalWords,
        const std::string& rawWord)
    {
        const std::string normalized = normalizeWord(rawWord);
        if (normalized.empty())
        {
            return;
        }

        ++counts[normalized];
        ++totalWords;
    }

    /*
     * Purpose: Replace current counts with the words parsed from supplied text.
     * Design: Treats every non-alphanumeric character as a separator to split punctuation compounds.
     * Workflow: Clear prior state, accumulate alphanumeric runs, and count each completed run.
     * Data Handoff: Consumes read-only text and stores normalized frequencies in wordCounts_.
     */
    void TargetMap::countText(const std::string& text)
    {
        clear();

        std::string word;
        for (const char character : text)
        {
            const unsigned char value = static_cast<unsigned char>(character);
            if (std::isalnum(value))
            {
                word.push_back(character);
            }
            else if (!word.empty())
            {
                addNormalizedWord(wordCounts_, totalWords_, word);
                word.clear();
            }
        }

        if (!word.empty())
        {
            addNormalizedWord(wordCounts_, totalWords_, word);
        }
    }

    /*
     * Purpose: Replace current counts with words from both fields of every Target.
     * Design: Builds one combined analysis while sharing the same parser used for typed text.
     * Workflow: Parse each first and second field into temporary counts, then merge them into this map.
     * Data Handoff: Reads a Target snapshot and stores its normalized field-word frequencies.
     */
    void TargetMap::countTargetFields(const std::vector<Target>& targets)
    {
        clear();

        for (const Target& target : targets)
        {
            const std::string combined = target.fieldOne() + " " + target.fieldTwo();
            std::string word;
            for (const char character : combined)
            {
                const unsigned char value = static_cast<unsigned char>(character);
                if (std::isalnum(value))
                {
                    word.push_back(character);
                }
                else if (!word.empty())
                {
                    addNormalizedWord(wordCounts_, totalWords_, word);
                    word.clear();
                }
            }

            if (!word.empty())
            {
                addNormalizedWord(wordCounts_, totalWords_, word);
            }
        }
    }

    /*
     * Purpose: Remove every stored word count.
     * Design: Resets both unique-key storage and the independent total counter.
     * Workflow: Clear the map and restore the total to zero.
     * Data Handoff: Discards all analysis state owned by this TargetMap.
     */
    void TargetMap::clear()
    {
        wordCounts_.clear();
        totalWords_ = 0;
    }

    /*
     * Purpose: Report whether no words have been counted.
     * Design: Uses map emptiness as the authoritative state check.
     * Workflow: Query wordCounts_ and return its empty flag.
     * Data Handoff: Returns a boolean guard for sessions and callers.
     */
    bool TargetMap::isEmpty() const
    {
        return wordCounts_.empty();
    }

    /*
     * Purpose: Report the total number of counted word occurrences.
     * Design: Returns the maintained total without rescanning frequencies.
     * Workflow: Read and return totalWords_.
     * Data Handoff: Gives callers the aggregate occurrence count.
     */
    int TargetMap::totalWords() const
    {
        return totalWords_;
    }

    /*
     * Purpose: Report the number of distinct normalized words.
     * Design: Uses the number of keys in the ordered map.
     * Workflow: Read and return wordCounts_.size().
     * Data Handoff: Gives callers the unique-word count.
     */
    std::size_t TargetMap::uniqueWords() const
    {
        return wordCounts_.size();
    }

    /*
     * Purpose: Find the largest stored frequency.
     * Design: Performs a linear scan because values, unlike keys, are not ordered.
     * Workflow: Start at zero and retain the greatest count encountered.
     * Data Handoff: Returns zero for an empty map or the maximum occurrence count.
     */
    int TargetMap::maxFrequency() const
    {
        int maximum = 0;
        for (const auto& entry : wordCounts_)
        {
            if (entry.second > maximum)
            {
                maximum = entry.second;
            }
        }
        return maximum;
    }

    /*
     * Purpose: Expose all frequencies without allowing external mutation.
     * Design: Returns the owned std::map by const reference to preserve ordering and avoid a copy.
     * Workflow: Return wordCounts_ directly.
     * Data Handoff: Gives callers read-only access to alphabetically ordered entries.
     */
    const std::map<std::string, int>& TargetMap::frequencies() const
    {
        return wordCounts_;
    }

    /*
     * Purpose: Return every word tied for the highest frequency.
     * Design: Computes the shared maximum once and preserves map iteration's alphabetical order.
     * Workflow: Find the maximum, collect each key with that count, and return the collection.
     * Data Handoff: Copies the most-frequent keys into a vector for display or testing.
     */
    std::vector<std::string> TargetMap::mostFrequentWords() const
    {
        const int maximum = maxFrequency();
        std::vector<std::string> words;

        for (const auto& entry : wordCounts_)
        {
            if (entry.second == maximum)
            {
                words.push_back(entry.first);
            }
        }

        return words;
    }
}
