/*
 * File: FileLoader.h
 * Description: Declares file loading helpers for website data and fallback entries.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Website: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Header.h"
#include "Website.h"
#include "WebsiteList.h"


// named container llb = linked list browser
namespace llb
{
    class FileLoader
    {
    public:
        static bool loadWebsitesFromFile(const std::string& filePath, WebsiteList& websites);
        static void loadFallbackWebsites(WebsiteList& websites);

    private:
        static std::string trim(const std::string& text);
    };
}
