/*
 * File: StackSession.h
 * Description: Declares the interactive session that wraps a TargetStack for the stack menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/TargetStack.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    class StackSession
    {
    private:
        TargetStack stack_;

    public:
        explicit StackSession(const std::vector<Target>& items);

        void display() const;
        void pushFromUser();
        void popAndShow();
        void peekAndShow() const;

        TargetStack& stack();
        const TargetStack& stack() const;
    };
}
