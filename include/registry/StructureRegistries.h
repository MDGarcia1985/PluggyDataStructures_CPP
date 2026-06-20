/*
 * File: StructureRegistries.h
 * Description: Declares per-structure operation registries built on the generic spine.
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
    class StackSession;
    class QueueSession;
    class TreeSession;
    class GraphSession;
    class HashTableSession;
    class MapSession;

    /*
     * Purpose: Provide an Exit-seeded, label-unique registry for any data structure session.
     * Design: Adds only registration policy on top of the shared OperationRegistry behavior.
     * Workflow: Seed a Back/Exit item on construction and validate each registered operation.
     * Data Handoff: Receives operation modules' registrations and serves them to a MenuController.
     */
    template <typename SessionT>
    class StructureRegistry : public OperationRegistry<SessionT>
    {
    public:
        /*
         * Purpose: Return the single registry instance for one session type.
         * Design: Uses a function-local static for thread-safe C++11 initialization.
         * Workflow: Construct on first access and return the same instance thereafter.
         * Data Handoff: Gives registration modules and menus shared registry access.
         */
        static StructureRegistry& instance()
        {
            static StructureRegistry registry;
            return registry;
        }

        /*
         * Purpose: Register one validated non-exit structure operation.
         * Design: Requires positive ids, unique labels, valid actions, and registry-owned Exit.
         * Workflow: Forward the operation through shared insertion policy and return its result.
         * Data Handoff: Moves operation metadata and callback into registry storage on success.
         */
        bool registerOperation(Operation<SessionT> operation)
        {
            return this->insertOperation(std::move(operation), true, true, true);
        }

    private:
        /*
         * Purpose: Initialize a structure registry with its standard Back operation.
         * Design: Keeps construction private so all callers share the singleton.
         * Workflow: Seed the registry-owned id-zero exit item during construction.
         * Data Handoff: Creates the terminal menu operation before module registrations run.
         */
        StructureRegistry()
        {
            this->seedItem(Operation<SessionT>{0, "Back to data structure menu", {}, true});
        }
    };

    using StackRegistry = StructureRegistry<StackSession>;
    using QueueRegistry = StructureRegistry<QueueSession>;
    using TreeRegistry = StructureRegistry<TreeSession>;
    using GraphRegistry = StructureRegistry<GraphSession>;
    using HashTableRegistry = StructureRegistry<HashTableSession>;
    using MapRegistry = StructureRegistry<MapSession>;
}
