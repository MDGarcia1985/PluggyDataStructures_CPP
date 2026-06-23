/*
 * File: TargetGraph.h
 * Description: Declares an integer-id graph of Target nodes with an adjacency list and traversals.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/Target.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    struct GraphEdge
    {
        std::size_t to;
        double weight;
        bool directed;
    };

    class TargetGraph
    {
    private:
        std::vector<Target> nodes_;
        std::vector<std::vector<GraphEdge>> adjacency_;
        bool hasDirectedEdges_ = false;

    public:
        std::size_t addNode(const Target& target);
        bool findId(const std::string& key, std::size_t& outId) const;
        bool addEdge(std::size_t from, std::size_t to, double weight, bool directed);
        bool addEdgeByKey(const std::string& fromKey, const std::string& toKey, double weight, bool directed);

        std::size_t nodeCount() const;
        bool isEmpty() const;
        bool hasDirectedEdges() const;
        const Target& node(std::size_t id) const;
        const std::vector<GraphEdge>& neighbors(std::size_t id) const;

        std::vector<Target> nodesSnapshot() const;
    };
}
