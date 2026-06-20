/*
 * File: TargetProgram.h
 * Description: Declares the application controller for generic target workflows.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/TargetList.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    class TargetProgram
    {
    private:
        TargetList targets_;
        bool exitRequested_;
        std::string dataFilePath_;

    public:
        explicit TargetProgram(std::string dataFilePath = DEFAULT_DATA_FILE);

        void setDataFilePath(std::string dataFilePath);
        void loadInitialData();
        void requestExit();
        bool exitRequested() const;

        TargetList& list();
        const TargetList& list() const;

        void displayList() const;
        void displayCurrent() const;
        void goForward();
        void goBackward();
        void addTargetFromUser();
        void deleteTargetFromUser();
        void findTargetFromUser();
    };

}
