/*
 * File: TargetStack.h
 * Description: Declares a last-in-first-out Target adapter backed by TargetList.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/TargetList.h"

namespace llb
{
    class TargetStack
    {
    private:
        TargetList stack_;

    public:
        void push(const Target& target);
        Target pop();
        Target peek() const;
        bool isEmpty() const;
        std::size_t size() const;
        void display() const;
    };
}
