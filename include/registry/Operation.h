/*
 * File: Operation.h
 * Description: Declares the generic, session-parameterized menu operation record.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Represent one selectable menu action for any data structure session.
     * Design: Parameterized on the session type so each structure reuses the same record shape.
     * Workflow: A registry stores these; a menu controller turns them into labels and runs the action.
     * Data Handoff: Carries a label and an action that receives the active SessionT by reference.
     */
    template <typename SessionT>
    struct Operation
    {
        int id = 0;
        std::string label;
        std::function<void(SessionT&)> action;
        bool isExit = false;
    };
}
