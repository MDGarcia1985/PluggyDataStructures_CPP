/*
 * File: Display.h
 * Description: Declares console output helpers for menus, lists, and search results.
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
    namespace Display
    {
        void printTitle();
        void printMessage(const std::string& message);
        void printWebsite(const Website& website, std::size_t position = 0);
        void printWebsiteList(const std::vector<Website>& websites);
        void printCurrentWebsite(const Website* website, std::size_t position);
        void printSearchResults(const std::vector<WebsiteSearchResult>& results);
    }
}
