/*
 * File: TargetGraph.cpp
 * Description: Implements integer-id graph storage, edge insertion, and BFS/DFS traversal.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "structures/TargetGraph.h"

#include "algorithms/common/KeyNormalization.h"

#include <cmath>


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Add a node and return its stable integer id.
     * Design: Stores nodes in a vector arena so an id is simply the vector index.
     * Workflow: Append the Target and a matching empty adjacency row, then return the new id.
     * Data Handoff: Receives a Target and hands back the id used by edges and traversals.
     */
    std::size_t TargetGraph::addNode(const Target& target)
    {
        nodes_.push_back(target);
        adjacency_.emplace_back();
        return nodes_.size() - 1;
    }

    /*
     * Purpose: Resolve a node id from its first-field key.
     * Design: Performs a case-insensitive linear scan over the node arena.
     * Workflow: Compare each node's normalized key, writing the id on the first match.
     * Data Handoff: Receives a key and reports the id through outId when found.
     */
    bool TargetGraph::findId(const std::string& key, std::size_t& outId) const
    {
        const std::string normalizedKey = normalizeKey(key);
        for (std::size_t id = 0; id < nodes_.size(); ++id)
        {
            if (normalizeKey(nodes_[id].fieldOne()) == normalizedKey)
            {
                outId = id;
                return true;
            }
        }
        return false;
    }

    /*
     * Purpose: Add an edge between two node ids.
     * Design: Stores a weighted neighbor entry, mirroring it when the edge is undirected.
     * Workflow: Validate ids, append the forward edge, and append the reverse edge when undirected.
     * Data Handoff: Receives node ids and a weight and updates the adjacency list.
     */
    bool TargetGraph::addEdge(std::size_t from, std::size_t to, double weight, bool directed)
    {
        if (from >= nodes_.size() || to >= nodes_.size() || weight < 0.0 || !std::isfinite(weight))
        {
            return false;
        }

        adjacency_[from].push_back(GraphEdge{to, weight, directed});
        if (directed)
        {
            hasDirectedEdges_ = true;
        }
        if (!directed && from != to)
        {
            adjacency_[to].push_back(GraphEdge{from, weight, false});
        }
        return true;
    }

    /*
     * Purpose: Add an edge between two nodes named by their first-field keys.
     * Design: Resolves both keys to ids before delegating to the id-based insertion.
     * Workflow: Look up both ids, then add the edge when both keys exist.
     * Data Handoff: Receives node keys and a weight and updates the adjacency list.
     */
    bool TargetGraph::addEdgeByKey(const std::string& fromKey, const std::string& toKey, double weight, bool directed)
    {
        std::size_t fromId = 0;
        std::size_t toId = 0;
        if (!findId(fromKey, fromId) || !findId(toKey, toId))
        {
            return false;
        }
        return addEdge(fromId, toId, weight, directed);
    }

    /*
     * Purpose: Report how many nodes the graph holds.
     * Design: Returns the arena size for constant-time access.
     * Workflow: Read the node vector size.
     * Data Handoff: Gives callers the node count for traversal sizing and display.
     */
    std::size_t TargetGraph::nodeCount() const
    {
        return nodes_.size();
    }

    /*
     * Purpose: Report whether the graph has no nodes.
     * Design: Uses the arena size for a constant-time check.
     * Workflow: Compare the node count to zero.
     * Data Handoff: Returns a boolean to callers before traversal.
     */
    bool TargetGraph::isEmpty() const
    {
        return nodes_.empty();
    }

    bool TargetGraph::hasDirectedEdges() const
    {
        return hasDirectedEdges_;
    }

    /*
     * Purpose: Read the Target stored at a node id.
     * Design: Returns a const reference into the arena for direct read access.
     * Workflow: Index the node vector by id.
     * Data Handoff: Supplies display and traversal code with node values.
     */
    const Target& TargetGraph::node(std::size_t id) const
    {
        return nodes_.at(id);
    }

    /*
     * Purpose: Read the outgoing edges for a node id.
     * Design: Returns a const reference to the adjacency row for direct iteration.
     * Workflow: Index the adjacency list by id.
     * Data Handoff: Supplies traversal and display code with a node's neighbors.
     */
    const std::vector<GraphEdge>& TargetGraph::neighbors(std::size_t id) const
    {
        return adjacency_.at(id);
    }

    /*
     * Purpose: Copy all node Targets into a vector.
     * Design: Keeps the arena private while exposing a simple snapshot for display.
     * Workflow: Return a copy of the node vector.
     * Data Handoff: Returns a std::vector<Target> snapshot to callers.
     */
    std::vector<Target> TargetGraph::nodesSnapshot() const
    {
        return nodes_;
    }
}
