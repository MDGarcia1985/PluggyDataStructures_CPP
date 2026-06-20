/*
 * File: MapSession.h
 * Description: Declares the interactive session for map-based word-frequency analysis.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/TargetMap.h"

#include <vector>


// named container llb = linked list browser
namespace llb
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
        void clear();

        TargetMap& map();
        const TargetMap& map() const;
    };
}
