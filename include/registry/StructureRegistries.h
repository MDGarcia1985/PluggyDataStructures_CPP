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


// named container llb = linked list browser
namespace llb
{
    class StackSession;
    class QueueSession;
    class TreeSession;
    class GraphSession;
    class HashTableSession;

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
        static StructureRegistry& instance()
        {
            static StructureRegistry registry;
            return registry;
        }

        bool registerOperation(Operation<SessionT> operation)
        {
            return this->insertOperation(std::move(operation), true, true, true);
        }

    private:
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
}
