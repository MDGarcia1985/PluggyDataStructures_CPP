/*
 * File: StackOperations.cpp
 * Description: Registers the interactive operations available in the stack menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "registry/StructureRegistries.h"
#include "session/StackSession.h"


using StackOp = pds::Operation<pds::StackSession>;

PDS_REGISTER_OPERATION(pds::StackRegistry::instance(),
    StackOp{1, "Display stack (top first)", [](pds::StackSession& session) { session.display(); }})

PDS_REGISTER_OPERATION(pds::StackRegistry::instance(),
    StackOp{2, "Push a target", [](pds::StackSession& session) { session.pushFromUser(); }})

PDS_REGISTER_OPERATION(pds::StackRegistry::instance(),
    StackOp{3, "Pop the top target", [](pds::StackSession& session) { session.popAndShow(); }})

PDS_REGISTER_OPERATION(pds::StackRegistry::instance(),
    StackOp{4, "Peek at the top target", [](pds::StackSession& session) { session.peekAndShow(); }})
