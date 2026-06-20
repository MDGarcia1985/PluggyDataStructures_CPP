/*
 * File: TreeOperations.cpp
 * Description: Registers the interactive operations available in the binary search tree menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "registry/StructureRegistries.h"
#include "session/TreeSession.h"


using TreeOp = pds::Operation<pds::TreeSession>;

LLB_REGISTER_OPERATION(pds::TreeRegistry::instance(),
    TreeOp{1, "Show in-order (sorted)", [](pds::TreeSession& session) { session.showInOrder(); }})

LLB_REGISTER_OPERATION(pds::TreeRegistry::instance(),
    TreeOp{2, "Show pre-order", [](pds::TreeSession& session) { session.showPreOrder(); }})

LLB_REGISTER_OPERATION(pds::TreeRegistry::instance(),
    TreeOp{3, "Show post-order", [](pds::TreeSession& session) { session.showPostOrder(); }})

LLB_REGISTER_OPERATION(pds::TreeRegistry::instance(),
    TreeOp{4, "Show level-order", [](pds::TreeSession& session) { session.showLevelOrder(); }})

LLB_REGISTER_OPERATION(pds::TreeRegistry::instance(),
    TreeOp{5, "Insert a target", [](pds::TreeSession& session) { session.insertFromUser(); }})

LLB_REGISTER_OPERATION(pds::TreeRegistry::instance(),
    TreeOp{6, "Find by key", [](pds::TreeSession& session) { session.findFromUser(); }})

LLB_REGISTER_OPERATION(pds::TreeRegistry::instance(),
    TreeOp{7, "Remove by key", [](pds::TreeSession& session) { session.removeFromUser(); }})

LLB_REGISTER_OPERATION(pds::TreeRegistry::instance(),
    TreeOp{8, "Show height and node count", [](pds::TreeSession& session) { session.showHeight(); }})
