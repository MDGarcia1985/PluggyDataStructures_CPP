/*
 * File: QueueOperations.cpp
 * Description: Registers the interactive operations available in the queue menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "registry/StructureRegistries.h"
#include "session/QueueSession.h"


using QueueOp = pds::Operation<pds::QueueSession>;

LLB_REGISTER_OPERATION(pds::QueueRegistry::instance(),
    QueueOp{1, "Display queue (front first)", [](pds::QueueSession& session) { session.display(); }})

LLB_REGISTER_OPERATION(pds::QueueRegistry::instance(),
    QueueOp{2, "Enqueue a target", [](pds::QueueSession& session) { session.enqueueFromUser(); }})

LLB_REGISTER_OPERATION(pds::QueueRegistry::instance(),
    QueueOp{3, "Dequeue the front target", [](pds::QueueSession& session) { session.dequeueAndShow(); }})

LLB_REGISTER_OPERATION(pds::QueueRegistry::instance(),
    QueueOp{4, "Peek at the front target", [](pds::QueueSession& session) { session.peekAndShow(); }})
