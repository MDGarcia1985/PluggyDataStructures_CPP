/*
 * File: TargetGraphTests.cpp
 * Description: Tests integer-id graph storage, edges, BFS/DFS traversal, and registry wiring.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "TestHarness.h"

#include "core/TargetGraph.h"
#include "io/FileLoader.h"
#include "registry/StructureRegistries.h"
#include "session/GraphSession.h"


using namespace pdstest;

namespace
{
    /*
     * Purpose: Join a traversal's Target keys into a compact assertion value.
     * Design: Uses comma separation so visit order remains visible in failures.
     * Workflow: Scan Targets, append separators after the first, and append each key.
     * Data Handoff: Converts a Target vector into one comparable string.
     */
    std::string keysOf(const std::vector<pds::Target>& targets)
    {
        std::string joined;
        for (const pds::Target& target : targets)
        {
            if (!joined.empty())
            {
                joined += ",";
            }
            joined += target.fieldOne();
        }
        return joined;
    }

    /*
     * Purpose: Build the shared connected graph fixture used by traversal tests.
     * Design: Uses deterministic nodes and undirected edges for stable visit orders.
     * Workflow: Add fixture nodes, connect them by key, and return the graph.
     * Data Handoff: Produces an independently owned TargetGraph for each caller.
     */
    pds::TargetGraph buildSampleGraph()
    {
        pds::TargetGraph graph;
        for (const char* key : {"A", "B", "C", "D", "E"})
        {
            graph.addNode(pds::Target(key, ""));
        }
        graph.addEdgeByKey("A", "B", 1.0, false);
        graph.addEdgeByKey("A", "C", 1.0, false);
        graph.addEdgeByKey("B", "D", 1.0, false);
        graph.addEdgeByKey("C", "D", 1.0, false);
        graph.addEdgeByKey("D", "E", 1.0, false);
        return graph;
    }
}

/*
 * Purpose: Verify graph node insertion, edge creation, and adjacency access.
 * Design: Builds a small weighted graph with resolvable node keys.
 * Workflow: Add nodes and edges, then assert counts and neighbor metadata.
 * Data Handoff: Sends Targets and edge values into the graph and reads adjacency views.
 */
PDS_TEST(testGraphNodesAndEdges)
{
    pds::TargetGraph graph = buildSampleGraph();

    expectEqual(graph.nodeCount(), 5, "Graph stores all added nodes.");

    std::size_t id = 0;
    expect(graph.findId("c", id), "Graph resolves a node id case-insensitively.");
    expectEqual(graph.node(id).fieldOne(), "C", "Resolved id maps to the matching node.");
    expect(!graph.findId("Z", id), "Graph reports a missing key.");

    std::size_t aId = 0;
    graph.findId("A", aId);
    expectEqual(graph.neighbors(aId).size(), 2, "Undirected edges populate the source adjacency.");

    expect(!graph.addEdgeByKey("A", "Z", 1.0, false), "Edge insertion fails for a missing endpoint.");
}

/*
 * Purpose: Verify breadth-first and depth-first traversal orders.
 * Design: Uses a deterministic graph whose visit sequences are easy to assert.
 * Workflow: Build connected nodes, run both traversals, and compare snapshots.
 * Data Handoff: Passes a start id into traversal methods and reads Target vectors.
 */
PDS_TEST(testGraphTraversals)
{
    pds::TargetGraph graph = buildSampleGraph();

    std::size_t start = 0;
    graph.findId("A", start);

    expectEqual(keysOf(graph.breadthFirst(start)), "A,B,C,D,E", "BFS visits nodes in breadth-first order.");

    const std::vector<pds::Target> dfsOrder = graph.depthFirst(start);
    expectEqual(dfsOrder.size(), 5, "DFS visits every reachable node once.");
    expectEqual(dfsOrder.front().fieldOne(), "A", "DFS starts at the requested node.");
}

/*
 * Purpose: Verify traversals terminate and visit each node once when cycles exist.
 * Design: Creates a cycle that requires visited-node tracking.
 * Workflow: Connect nodes cyclically, traverse, and assert the bounded result.
 * Data Handoff: Sends cyclic adjacency into traversal and reads de-duplicated visits.
 */
PDS_TEST(testGraphHandlesCycles)
{
    pds::TargetGraph graph;
    graph.addNode(pds::Target("A", ""));
    graph.addNode(pds::Target("B", ""));
    graph.addNode(pds::Target("C", ""));
    graph.addEdgeByKey("A", "B", 1.0, false);
    graph.addEdgeByKey("B", "C", 1.0, false);
    graph.addEdgeByKey("C", "A", 1.0, false);

    std::size_t start = 0;
    graph.findId("A", start);

    expectEqual(graph.breadthFirst(start).size(), 3, "BFS terminates on a cyclic graph.");
    expectEqual(graph.depthFirst(start).size(), 3, "DFS terminates on a cyclic graph.");
}

/*
 * Purpose: Verify graph session edge loading and operation registration.
 * Design: Covers integration among node snapshots, edge records, and GraphRegistry.
 * Workflow: Construct a session, inspect graph state, and validate menu operations.
 * Data Handoff: Routes loaded nodes and edges into the session and reads integration state.
 */
PDS_TEST(testGraphSessionAndRegistry)
{
    std::vector<pds::Target> nodes;
    nodes.push_back(pds::Target("A", ""));
    nodes.push_back(pds::Target("B", ""));

    std::vector<pds::EdgeRecord> edges;
    edges.push_back(pds::EdgeRecord{"A", "B", 1.0, false});
    edges.push_back(pds::EdgeRecord{"A", "Missing", 1.0, false});

    pds::GraphSession session(nodes, edges);
    expectEqual(session.graph().nodeCount(), 2, "Graph session loads all nodes.");
    expectEqual(session.unresolvedEdgeCount(), 1,
        "Graph session counts edge records whose endpoint keys cannot be resolved.");

    std::size_t aId = 0;
    session.graph().findId("A", aId);
    expectEqual(session.graph().neighbors(aId).size(), 1, "Graph session applies loaded edges.");

    const auto operations = pds::GraphRegistry::instance().operations();
    expect(operations.size() >= 2, "Graph registry holds registered operations.");
    expect(operations.back().isExit, "Graph registry keeps Exit last.");
}
