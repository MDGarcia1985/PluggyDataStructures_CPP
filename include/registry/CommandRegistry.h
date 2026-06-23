/*
 * File: CommandRegistry.h
 * Description: Declares target-menu command registration and ordering.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "registry/OperationRegistry.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    class TargetProgram;

    using CommandPlugin = Operation<TargetProgram>;

    class CommandRegistry : public OperationRegistry<TargetProgram>
    {
    public:
        static CommandRegistry& instance();

        /*
         * Purpose: Register a valid command for the Target Data Structure Menu.
         * Design: Requires a positive id and a single Exit while reusing shared insertion policy.
         * Workflow: Forward the command to the generic insert routine with command-specific rules.
         * Data Handoff: Receives command metadata from a command module and stores it on success.
         */
        bool registerCommand(CommandPlugin command)
        {
            return insertOperation(std::move(command), true, false, false);
        }

        /*
         * Purpose: Return target commands in stable menu order with Exit last.
         * Design: Delegates to the shared ordering policy so all registries behave consistently.
         * Workflow: Forward to the base operations() helper.
         * Data Handoff: Supplies TargetDataStructureMenu with its executable menu entries.
         */
        std::vector<CommandPlugin> commands() const
        {
            return operations();
        }

    private:
        CommandRegistry() = default;
    };
}

#define PDS_REGISTER_COMMAND(ID, LABEL, FUNCTION)                                      \
    namespace                                                                          \
    {                                                                                  \
        const bool PDS_DETAIL_CONCAT(pds_registered_command_, __LINE__) =              \
            ::pds::CommandRegistry::instance().registerCommand(                        \
                ::pds::CommandPlugin{ID, LABEL, FUNCTION});                            \
    }

#define PDS_REGISTER_EXIT_COMMAND(ID, LABEL, FUNCTION)                                 \
    namespace                                                                          \
    {                                                                                  \
        const bool PDS_DETAIL_CONCAT(pds_registered_exit_command_, __LINE__) =         \
            ::pds::CommandRegistry::instance().registerCommand(                        \
                ::pds::CommandPlugin{ID, LABEL, FUNCTION, true});                      \
    }
