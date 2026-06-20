/*
 * File: GraphSession.h
 * Description: Declares the interactive session that wraps a TargetGraph for the graph menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/TargetGraph.h"
#include "io/FileLoader.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    class GraphSession
    {
    private:
        TargetGraph graph_;
        std::size_t unresolvedEdgeCount_;

    public:
        GraphSession(const std::vector<Target>& nodes, const std::vector<EdgeRecord>& edges);

        void showNodes() const;
        void showAdjacency() const;
        void breadthFirstFromUser() const;
        void depthFirstFromUser() const;
        void addEdgeFromUser();

        std::size_t unresolvedEdgeCount() const;
        TargetGraph& graph();
        const TargetGraph& graph() const;
    };
}
