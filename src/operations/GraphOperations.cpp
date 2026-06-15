/*
 * File: GraphOperations.cpp
 * Description: Registers the interactive operations available in the graph menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "registry/StructureRegistries.h"
#include "session/GraphSession.h"


using GraphOp = llb::Operation<llb::GraphSession>;

LLB_REGISTER_OPERATION(llb::GraphRegistry::instance(),
    GraphOp{1, "Show nodes", [](llb::GraphSession& session) { session.showNodes(); }})

LLB_REGISTER_OPERATION(llb::GraphRegistry::instance(),
    GraphOp{2, "Show adjacency list", [](llb::GraphSession& session) { session.showAdjacency(); }})

LLB_REGISTER_OPERATION(llb::GraphRegistry::instance(),
    GraphOp{3, "Breadth-first search from node", [](llb::GraphSession& session) { session.breadthFirstFromUser(); }})

LLB_REGISTER_OPERATION(llb::GraphRegistry::instance(),
    GraphOp{4, "Depth-first search from node", [](llb::GraphSession& session) { session.depthFirstFromUser(); }})

LLB_REGISTER_OPERATION(llb::GraphRegistry::instance(),
    GraphOp{5, "Add an edge", [](llb::GraphSession& session) { session.addEdgeFromUser(); }})
