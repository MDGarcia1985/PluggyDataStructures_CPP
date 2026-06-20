/*
 * File: GraphSession.cpp
 * Description: Implements interactive graph operations and traversals over a TargetGraph.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "session/GraphSession.h"
#include "ui/Display.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Build a graph session from loaded nodes and edge records.
     * Design: Adds every node first so edges can resolve their endpoints by key.
     * Workflow: Add each node, then add each edge by its from/to keys with its weight and direction.
     * Data Handoff: Receives node and edge snapshots and fills the backing TargetGraph.
     */
    GraphSession::GraphSession(const std::vector<Target>& nodes, const std::vector<EdgeRecord>& edges)
        : graph_(), unresolvedEdgeCount_(0)
    {
        for (const Target& node : nodes)
        {
            graph_.addNode(node);
        }

        for (const EdgeRecord& edge : edges)
        {
            if (!graph_.addEdgeByKey(edge.from, edge.to, edge.weight, edge.directed))
            {
                ++unresolvedEdgeCount_;
            }
        }
    }

    /*
     * Purpose: Show every node with its id.
     * Design: Lists the integer-id arena so users can reference nodes by key or position.
     * Workflow: Print each node id alongside its display text.
     * Data Handoff: Reads the node snapshot and writes it to the console.
     */
    void GraphSession::showNodes() const
    {
        if (graph_.isEmpty())
        {
            Display::printMessage("The graph has no nodes.");
            return;
        }

        std::cout << "\nGraph nodes:\n";
        for (std::size_t id = 0; id < graph_.nodeCount(); ++id)
        {
            std::cout << "[" << id << "] " << graph_.node(id).toDisplayString() << '\n';
        }
    }

    /*
     * Purpose: Show each node's outgoing edges.
     * Design: Prints the adjacency list so the graph structure is visible at a glance.
     * Workflow: For each node, print its key and each neighbor key with the edge weight.
     * Data Handoff: Reads node and neighbor data and writes the adjacency view to the console.
     */
    void GraphSession::showAdjacency() const
    {
        if (graph_.isEmpty())
        {
            Display::printMessage("The graph has no nodes.");
            return;
        }

        std::cout << "\nAdjacency list:\n";
        for (std::size_t id = 0; id < graph_.nodeCount(); ++id)
        {
            std::cout << graph_.node(id).fieldOne() << " ->";

            const std::vector<GraphEdge>& edges = graph_.neighbors(id);
            if (edges.empty())
            {
                std::cout << " (none)";
            }
            else
            {
                for (const GraphEdge& edge : edges)
                {
                    std::cout << ' ' << graph_.node(edge.to).fieldOne()
                        << "(w=" << edge.weight << ')';
                }
            }

            std::cout << '\n';
        }
    }

    /*
     * Purpose: Run a breadth-first traversal from a user-chosen node.
     * Design: Resolves a start key to an id before delegating to the graph traversal.
     * Workflow: Prompt for a key, resolve its id, run BFS, and display the visit order.
     * Data Handoff: Moves user text into the graph's BFS and shows the resulting order.
     */
    void GraphSession::breadthFirstFromUser() const
    {
        std::string key;
        std::cout << "\nEnter start node key: ";
        std::getline(std::cin, key);

        std::size_t startId = 0;
        if (!graph_.findId(key, startId))
        {
            Display::printMessage("No node matches that key.");
            return;
        }

        Display::printTargets("Breadth-first order", graph_.breadthFirst(startId));
    }

    /*
     * Purpose: Run a depth-first traversal from a user-chosen node.
     * Design: Resolves a start key to an id before delegating to the graph traversal.
     * Workflow: Prompt for a key, resolve its id, run DFS, and display the visit order.
     * Data Handoff: Moves user text into the graph's DFS and shows the resulting order.
     */
    void GraphSession::depthFirstFromUser() const
    {
        std::string key;
        std::cout << "\nEnter start node key: ";
        std::getline(std::cin, key);

        std::size_t startId = 0;
        if (!graph_.findId(key, startId))
        {
            Display::printMessage("No node matches that key.");
            return;
        }

        Display::printTargets("Depth-first order", graph_.depthFirst(startId));
    }

    /*
     * Purpose: Add an undirected edge between two existing nodes from user input.
     * Design: Uses node keys so the user does not need to know internal ids.
     * Workflow: Prompt for two keys, add the edge, and report success or a missing-node error.
     * Data Handoff: Moves user text into the graph's key-based edge insertion.
     */
    void GraphSession::addEdgeFromUser()
    {
        std::string fromKey;
        std::string toKey;

        std::cout << "\nEnter source node key: ";
        std::getline(std::cin, fromKey);
        std::cout << "Enter destination node key: ";
        std::getline(std::cin, toKey);

        if (graph_.addEdgeByKey(fromKey, toKey, 1.0, false))
        {
            Display::printMessage("Added undirected edge.");
        }
        else
        {
            Display::printMessage("Could not add edge; check that both node keys exist.");
        }
    }

    /*
     * Purpose: Report how many loaded edge records referenced missing node keys.
     * Design: Counts rejected records during construction without coupling FileLoader to graph nodes.
     * Workflow: Return the cached count accumulated while applying parsed edge records.
     * Data Handoff: Supplies StructureMenu with endpoint-resolution diagnostics.
     */
    std::size_t GraphSession::unresolvedEdgeCount() const
    {
        return unresolvedEdgeCount_;
    }

    /*
     * Purpose: Provide mutable access to the backing graph.
     * Design: Exposes the graph for tests and advanced operations.
     * Workflow: Return graph_ by reference.
     * Data Handoff: Gives callers direct access to the session's TargetGraph.
     */
    TargetGraph& GraphSession::graph()
    {
        return graph_;
    }

    /*
     * Purpose: Provide read-only access to the backing graph.
     * Design: Supports const sessions without allowing mutation.
     * Workflow: Return graph_ by const reference.
     * Data Handoff: Gives callers read access to the session's TargetGraph.
     */
    const TargetGraph& GraphSession::graph() const
    {
        return graph_;
    }
}
