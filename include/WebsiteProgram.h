/*
 * File: WebsiteProgram.h
 * Description: Declares the application controller for website browser workflows.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Website: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Header.h"
#include "WebsiteList.h"


// named container llb = linked list browser
namespace llb
{
    class WebsiteProgram
    {
    private:
        WebsiteList websites_;
        bool exitRequested_;
        std::string dataFilePath_;

    public:
        explicit WebsiteProgram(std::string dataFilePath = DEFAULT_DATA_FILE);

        void run();
        void loadInitialData();
        void requestExit();
        bool exitRequested() const;

        WebsiteList& list();
        const WebsiteList& list() const;

        void displayList() const;
        void displayCurrent() const;
        void goForward();
        void goBackward();
        void addWebsiteFromUser();
        void deleteWebsiteFromUser();
        void findWebsiteFromUser();
    };

    int runApp();
}
