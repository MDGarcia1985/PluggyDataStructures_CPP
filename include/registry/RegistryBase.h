/*
 * File: RegistryBase.h
 * Description: Declares shared storage behavior for plugin registries.
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
    template <typename Item>
    class RegistryBase
    {
    protected:
        /*
         * Purpose: Store an item after a derived registry has validated it.
         * Design: Centralizes ownership and move behavior for all registry implementations.
         * Workflow: Move the supplied item into the shared item vector and report success.
         * Data Handoff: Receives one validated registration and retains it for derived registries.
         */
        bool append(Item item)
        {
            items_.push_back(std::move(item));
            return true;
        }

        /*
         * Purpose: Give derived registries read-only access to their shared storage.
         * Design: Returns a const reference so validation and ordering code cannot replace storage.
         * Workflow: Return the internally owned item vector.
         * Data Handoff: Supplies registered items to derived lookup and ordering operations.
         */
        const std::vector<Item>& storedItems() const
        {
            return items_;
        }

    private:
        std::vector<Item> items_;
    };
}

#define LLB_DETAIL_CONCAT_IMPL(first, second) first##second
#define LLB_DETAIL_CONCAT(first, second) LLB_DETAIL_CONCAT_IMPL(first, second)
