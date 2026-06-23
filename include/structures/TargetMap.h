/*
 * File: TargetMap.h
 * Description: Declares the alphabetically ordered word-frequency map.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Target.h"

#include <cstddef>
#include <map>
#include <string>
#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    class TargetMap
    {
    private:
        std::map<std::string, int> wordCounts_;
        int totalWords_;

        static std::string normalizeWord(const std::string& word);
        static void addNormalizedWord(
            std::map<std::string, int>& counts,
            int& totalWords,
            const std::string& rawWord);

    public:
        TargetMap();

        void countText(const std::string& text);
        void countTargetFields(const std::vector<Target>& targets);
        void clear();

        bool isEmpty() const;
        int totalWords() const;
        std::size_t uniqueWords() const;
        int maxFrequency() const;

        const std::map<std::string, int>& frequencies() const;
        const std::map<std::string, int>& entries() const;
        int countFor(const std::string& word) const;
        bool contains(const std::string& word) const;
        std::size_t size() const;
        std::vector<std::string> mostFrequentWords() const;
    };
}
