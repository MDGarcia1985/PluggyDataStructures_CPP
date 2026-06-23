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


using GraphOp = pds::Operation<pds::GraphSession>;

PDS_REGISTER_OPERATION(pds::GraphRegistry::instance(),
    GraphOp{1, "Show nodes", [](pds::GraphSession& session) { session.showNodes(); }})

PDS_REGISTER_OPERATION(pds::GraphRegistry::instance(),
    GraphOp{2, "Show adjacency list", [](pds::GraphSession& session) { session.showAdjacency(); }})

PDS_REGISTER_OPERATION(pds::GraphRegistry::instance(),
    GraphOp{3, "Breadth-first search from node", [](pds::GraphSession& session) { session.breadthFirstFromUser(); }})

PDS_REGISTER_OPERATION(pds::GraphRegistry::instance(),
    GraphOp{4, "Depth-first search from node", [](pds::GraphSession& session) { session.depthFirstFromUser(); }})

PDS_REGISTER_OPERATION(pds::GraphRegistry::instance(),
    GraphOp{5, "Add an edge", [](pds::GraphSession& session) { session.addEdgeFromUser(); }})

PDS_REGISTER_OPERATION(pds::GraphRegistry::instance(),
    GraphOp{6, "Path exists between nodes", [](pds::GraphSession& session) { session.pathExistsFromUser(); }})

PDS_REGISTER_OPERATION(pds::GraphRegistry::instance(),
    GraphOp{7, "Shortest path (weighted)", [](pds::GraphSession& session) { session.shortestPathFromUser(); }})

PDS_REGISTER_OPERATION(pds::GraphRegistry::instance(),
    GraphOp{8, "Show node degree", [](pds::GraphSession& session) { session.showDegreeFromUser(); }})

PDS_REGISTER_OPERATION(pds::GraphRegistry::instance(),
    GraphOp{9, "Show components and cycle", [](pds::GraphSession& session) { session.showStructure(); }})
