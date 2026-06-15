/*
 * File: HashTableSession.h
 * Description: Declares the interactive session that wraps a TargetHashTable for the hash menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/TargetHashTable.h"


// named container llb = linked list browser
namespace llb
{
    class HashTableSession
    {
    private:
        TargetHashTable table_;

    public:
        explicit HashTableSession(const std::vector<Target>& items);

        void insertFromUser();
        void findFromUser() const;
        void eraseFromUser();
        void showBuckets() const;
        void showLoadFactor() const;

        TargetHashTable& table();
        const TargetHashTable& table() const;
    };
}
