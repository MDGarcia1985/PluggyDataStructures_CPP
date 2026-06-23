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
#include "structures/TargetHashTable.h"


// named container pds = Pluggy Data Structures
namespace pds
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
        void switchStrategy();
        void showCollisionMetrics() const;

        TargetHashTable& table();
        const TargetHashTable& table() const;
    };
}
