/*
 * File: DataSourceMenu.cpp
 * Description: Implements dynamic dataset discovery and type-aware selection.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "io/FileLoader.h"
#include "ui/DataSourceMenu.h"
#include "ui/Menu.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Let the user choose a supported dataset discovered in data/.
     * Design: Keeps file discovery and type decisions out of the generic Menu helper.
     * Workflow: Discover files, build labels, select a valid position, reject unsupported types, and repeat.
     * Data Handoff: Returns a supported file path to MainMenu or an empty path when no files exist.
     */
    std::string DataSourceMenu::selectDataSource()
    {
        while (true)
        {
            const std::vector<std::string> files = FileLoader::discoverDataFiles();
            if (files.empty())
            {
                std::cout << "\nNo data files were found in " << DATA_DIRECTORY << ".\n";
                return "";
            }

            std::vector<std::string> labels;
            labels.reserve(files.size());
            for (const std::string& file : files)
            {
                std::string label = std::filesystem::path(file).filename().string();
                if (FileLoader::fileType(file) == DataFileType::Unsupported)
                {
                    label += " (unsupported)";
                }
                labels.push_back(std::move(label));
            }

            const std::size_t selectedIndex = Menu::select("Available Data Files:", labels);
            const std::string& selectedPath = files[selectedIndex];

            if (FileLoader::fileType(selectedPath) == DataFileType::Unsupported)
            {
                std::cout << "Unsupported file type. Please choose a .txt or .csv file.\n";
                continue;
            }

            return selectedPath;
        }
    }
}
