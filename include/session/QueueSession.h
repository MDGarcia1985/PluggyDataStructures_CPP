/*
 * File: QueueSession.h
 * Description: Declares the interactive session that wraps a TargetQueue for the queue menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/TargetQueue.h"


// named container llb = linked list browser
namespace llb
{
    class QueueSession
    {
    private:
        TargetQueue queue_;

    public:
        explicit QueueSession(const std::vector<Target>& items);

        void display() const;
        void enqueueFromUser();
        void dequeueAndShow();
        void peekAndShow() const;

        TargetQueue& queue();
        const TargetQueue& queue() const;
    };
}
