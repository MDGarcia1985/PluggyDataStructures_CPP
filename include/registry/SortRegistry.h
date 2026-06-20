/*
 * File: SortRegistry.h
 * Description: Declares sorting algorithm registration and ordering.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "registry/OperationRegistry.h"


// named container llb = linked list browser
namespace llb
{
    class TargetProgram;

    using SortCommand = Operation<TargetProgram>;

    class SortRegistry : public OperationRegistry<TargetProgram>
    {
    public:
        static SortRegistry& instance();

        /*
         * Purpose: Register one sorting algorithm.
         * Design: Rejects Exit registrations and duplicate labels while preserving registration order.
         * Workflow: Forward to the shared insert routine with sort-specific rules.
         * Data Handoff: Receives sort metadata from an algorithm module and stores it on success.
         */
        bool registerCommand(SortCommand command)
        {
            return insertOperation(std::move(command), false, true, true);
        }

        /*
         * Purpose: Return sort commands with the Exit entry at the end.
         * Design: Delegates to the shared ordering policy that keeps registration order stable.
         * Workflow: Forward to the base operations() helper.
         * Data Handoff: Supplies SortTypeMenu with ordered labels and actions.
         */
        std::vector<SortCommand> commands() const
        {
            return operations();
        }

    private:
        SortRegistry();
    };
}

#define LLB_REGISTER_SORT(LABEL, FUNCTION)                                             \
    namespace                                                                          \
    {                                                                                  \
        const bool LLB_DETAIL_CONCAT(llb_registered_sort_, __LINE__) =                 \
            ::llb::SortRegistry::instance().registerCommand(                           \
                ::llb::SortCommand{0, LABEL, FUNCTION});                               \
    }
