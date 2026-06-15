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


using namespace llbtest;

namespace
{
    std::string keysOf(const std::vector<llb::Target>& targets)
    {
        std::string joined;
        for (const llb::Target& target : targets)
        {
            if (!joined.empty())
            {
                joined += ",";
            }
            joined += target.fieldOne();
        }
        return joined;
    }

    llb::TargetGraph buildSampleGraph()
    {
        llb::TargetGraph graph;
        for (const char* key : {"A", "B", "C", "D", "E"})
        {
            graph.addNode(llb::Target(key, ""));
        }
        graph.addEdgeByKey("A", "B", 1.0, false);
        graph.addEdgeByKey("A", "C", 1.0, false);
        graph.addEdgeByKey("B", "D", 1.0, false);
        graph.addEdgeByKey("C", "D", 1.0, false);
        graph.addEdgeByKey("D", "E", 1.0, false);
        return graph;
    }
}

LLB_TEST(testGraphNodesAndEdges)
{
    llb::TargetGraph graph = buildSampleGraph();

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

LLB_TEST(testGraphTraversals)
{
    llb::TargetGraph graph = buildSampleGraph();

    std::size_t start = 0;
    graph.findId("A", start);

    expectEqual(keysOf(graph.breadthFirst(start)), "A,B,C,D,E", "BFS visits nodes in breadth-first order.");

    const std::vector<llb::Target> dfsOrder = graph.depthFirst(start);
    expectEqual(dfsOrder.size(), 5, "DFS visits every reachable node once.");
    expectEqual(dfsOrder.front().fieldOne(), "A", "DFS starts at the requested node.");
}

LLB_TEST(testGraphHandlesCycles)
{
    llb::TargetGraph graph;
    graph.addNode(llb::Target("A", ""));
    graph.addNode(llb::Target("B", ""));
    graph.addNode(llb::Target("C", ""));
    graph.addEdgeByKey("A", "B", 1.0, false);
    graph.addEdgeByKey("B", "C", 1.0, false);
    graph.addEdgeByKey("C", "A", 1.0, false);

    std::size_t start = 0;
    graph.findId("A", start);

    expectEqual(graph.breadthFirst(start).size(), 3, "BFS terminates on a cyclic graph.");
    expectEqual(graph.depthFirst(start).size(), 3, "DFS terminates on a cyclic graph.");
}

LLB_TEST(testGraphSessionAndRegistry)
{
    std::vector<llb::Target> nodes;
    nodes.push_back(llb::Target("A", ""));
    nodes.push_back(llb::Target("B", ""));

    std::vector<llb::EdgeRecord> edges;
    edges.push_back(llb::EdgeRecord{"A", "B", 1.0, false});
    edges.push_back(llb::EdgeRecord{"A", "Missing", 1.0, false});

    llb::GraphSession session(nodes, edges);
    expectEqual(session.graph().nodeCount(), 2, "Graph session loads all nodes.");
    expectEqual(session.unresolvedEdgeCount(), 1,
        "Graph session counts edge records whose endpoint keys cannot be resolved.");

    std::size_t aId = 0;
    session.graph().findId("A", aId);
    expectEqual(session.graph().neighbors(aId).size(), 1, "Graph session applies loaded edges.");

    const auto operations = llb::GraphRegistry::instance().operations();
    expect(operations.size() >= 2, "Graph registry holds registered operations.");
    expect(operations.back().isExit, "Graph registry keeps Exit last.");
}
