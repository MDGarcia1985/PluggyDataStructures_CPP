/*
 * File: MapSession.h
 * Description: Declares the interactive session for map-based word-frequency analysis.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "structures/TargetMap.h"

#include <vector>


// named container pds = Pluggy Data Structures
namespace pds
{
    class MapSession
    {
    private:
        TargetMap map_;
        std::vector<Target> loadedTargets_;

    public:
        explicit MapSession(const std::vector<Target>& items);

        void countWordsFromUser();
        void countWordsFromLoadedTargets();
        void showWordFrequencies() const;
        void showMostFrequentWords() const;
        void showSummary() const;
        void showTopNFromUser() const;
        void showLeastFrequentFromUser() const;
        void showFrequencyBuckets() const;
        void searchWordFromUser() const;
        void showWordsStartingWithFromUser() const;
        void showFrequencyRanking() const;
        void showAlphabeticalRanking() const;
        void clear();

        TargetMap& map();
        const TargetMap& map() const;
    };
}
