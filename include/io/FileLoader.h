/*
 * File: FileLoader.h
 * Description: Declares file loading helpers for generic target data and fallback entries.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/Target.h"
#include "core/TargetList.h"


// named container llb = linked list browser
namespace llb
{
    enum class DataFileType
    {
        Text,
        Csv,
        Unsupported
    };

    struct EdgeRecord
    {
        std::string from;
        std::string to;
        double weight = 1.0;
        bool directed = false;
    };

    class FileLoader
    {
    public:
        static std::vector<std::string> discoverDataFiles();
        static DataFileType fileType(const std::string& filePath);
        static bool loadTargetsFromFile(const std::string& filePath, TargetList& targets);
        static void loadFallbackTargets(TargetList& targets);

        static std::string discoverEdgeFile(const std::string& nodeFilePath);
        static std::vector<EdgeRecord> loadEdges(const std::string& edgeFilePath);

    private:
        static std::string trim(const std::string& text);
        static std::vector<std::string> parseCsvRow(const std::string& line);
        static bool loadTextTargets(const std::string& filePath, TargetList& targets);
        static bool loadCsvTargets(const std::string& filePath, TargetList& targets);
    };
}
