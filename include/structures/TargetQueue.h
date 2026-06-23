/*
 * File: TargetQueue.h
 * Description: Declares a first-in-first-out Target adapter backed by TargetList.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/TargetList.h"

namespace pds
{
    class TargetQueue
    {
    private:
        TargetList queue_;

    public:
        void enqueue(const Target& target);
        Target dequeue();
        Target peek() const;
        bool isEmpty() const;
        std::size_t size() const;
        void display() const;
    };
}
