/*
 * File: OperationRegistry.h
 * Description: Declares shared validation, ordering, and lookup for session operation registries.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "registry/Operation.h"
#include "registry/RegistryBase.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Provide reusable policy for every concrete operation registry.
     * Design: Builds on RegistryBase storage and adds ordering, lookup, and guarded insertion.
     * Workflow: Concrete registries seed an Exit item, insert validated operations, and read ordered copies.
     * Data Handoff: Receives Operation<SessionT> entries and hands ordered copies to menu controllers.
     */
    template <typename SessionT>
    class OperationRegistry : protected RegistryBase<Operation<SessionT>>
    {
    public:
        using OperationType = Operation<SessionT>;

        /*
         * Purpose: Return all operations in stable menu order with Exit last.
         * Design: Sorts a copy so stored registration data is never mutated.
         * Workflow: Copy stored items, keep regular entries first by id, and move Exit to the end.
         * Data Handoff: Supplies a menu controller with the ordered labels and actions to display.
         */
        std::vector<OperationType> operations() const
        {
            std::vector<OperationType> ordered = this->storedItems();
            std::stable_sort(ordered.begin(), ordered.end(),
                [](const OperationType& left, const OperationType& right)
                {
                    if (left.isExit != right.isExit)
                    {
                        return !left.isExit;
                    }
                    return left.id < right.id;
                });
            return ordered;
        }

        /*
         * Purpose: Find a registered operation by its stable id.
         * Design: Returns nullptr for absent ids rather than throwing or exposing storage.
         * Workflow: Scan stored operations and return the matching address when found.
         * Data Handoff: Gives tests and non-menu callers read-only access to one operation.
         */
        const OperationType* findById(int id) const
        {
            for (const OperationType& item : this->storedItems())
            {
                if (item.id == id)
                {
                    return &item;
                }
            }

            return nullptr;
        }

    protected:
        /*
         * Purpose: Insert one operation after applying the requested validation policy.
         * Design: Centralizes empty-label, missing-action, id, duplicate, and single-Exit rules.
         * Workflow: Validate fields and scan stored items for conflicts before appending.
         * Data Handoff: Receives an operation from a concrete registry and stores it on success.
         */
        bool insertOperation(OperationType item, bool requirePositiveId, bool requireUniqueLabel, bool rejectExit)
        {
            if (item.label.empty())
            {
                return false;
            }

            if (rejectExit && item.isExit)
            {
                return false;
            }

            if (!item.isExit && !item.action)
            {
                return false;
            }

            if (requirePositiveId && item.id < 1)
            {
                return false;
            }

            for (const OperationType& existing : this->storedItems())
            {
                if (requirePositiveId && existing.id == item.id)
                {
                    return false;
                }
                if (requireUniqueLabel && existing.label == item.label)
                {
                    return false;
                }
                if (existing.isExit && item.isExit)
                {
                    return false;
                }
            }

            return this->append(std::move(item));
        }

        /*
         * Purpose: Append a registry-owned item without applying registration validation.
         * Design: Lets a concrete registry seed its own Exit entry during construction.
         * Workflow: Forward the prepared item directly to shared storage.
         * Data Handoff: Stores a constructor-supplied operation such as the Exit entry.
         */
        void seedItem(OperationType item)
        {
            this->append(std::move(item));
        }
    };
}

#define LLB_REGISTER_OPERATION(REGISTRY, ...)                                          \
    namespace                                                                          \
    {                                                                                  \
        const bool LLB_DETAIL_CONCAT(llb_registered_operation_, __LINE__) =            \
            (REGISTRY).registerOperation(__VA_ARGS__);                                 \
    }
