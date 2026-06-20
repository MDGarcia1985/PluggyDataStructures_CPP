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


// named container pds = Pluggy Data Structures
namespace pds
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

    enum class EdgeLoadStatus
    {
        NotFound,
        Loaded,
        Empty,
        Invalid
    };

    struct EdgeLoadResult
    {
        EdgeLoadStatus status = EdgeLoadStatus::NotFound;
        std::vector<EdgeRecord> edges;
        std::size_t skippedRowCount = 0;
    };

    class FileLoader
    {
    public:
        static std::vector<std::string> discoverDataFiles();
        static DataFileType fileType(const std::string& filePath);
        static bool loadTargetsFromFile(const std::string& filePath, TargetList& targets);
        static bool loadTargetsOrFallback(const std::string& filePath, TargetList& targets);
        static void loadFallbackTargets(TargetList& targets);

        static std::string discoverEdgeFile(const std::string& nodeFilePath);
        static EdgeLoadResult loadEdges(const std::string& edgeFilePath);

    private:
        static std::string trim(const std::string& text);
        static std::vector<std::string> parseCsvRow(const std::string& line);
        static bool loadTextTargets(const std::string& filePath, TargetList& targets);
        static bool loadCsvTargets(const std::string& filePath, TargetList& targets);
    };
}
