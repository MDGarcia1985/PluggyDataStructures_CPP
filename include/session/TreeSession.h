/*
 * File: TreeSession.h
 * Description: Declares the interactive session that wraps a TargetTree for the tree menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/TargetTree.h"


// named container llb = linked list browser
namespace llb
{
    class TreeSession
    {
    private:
        TargetTree tree_;

    public:
        explicit TreeSession(const std::vector<Target>& items);

        void insertFromUser();
        void findFromUser() const;
        void removeFromUser();
        void showInOrder() const;
        void showPreOrder() const;
        void showPostOrder() const;
        void showLevelOrder() const;
        void showHeight() const;

        TargetTree& tree();
        const TargetTree& tree() const;
    };
}
