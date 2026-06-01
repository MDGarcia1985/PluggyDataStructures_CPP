/*
 * File: FileLoader.h
 * Description: Declares file loading helpers for generic target data and fallback entries.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Header.h"
#include "Target.h"
#include "TargetList.h"


// named container llb = linked list browser
namespace llb
{
    class FileLoader
    {
    public:
        static bool loadTargetsFromFile(const std::string& filePath, TargetList& targets);
        static void loadFallbackTargets(TargetList& targets);

    private:
        static std::string trim(const std::string& text);
    };
}
