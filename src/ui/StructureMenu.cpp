/*
 * File: StructureMenu.cpp
 * Description: Implements top-level data structure selection and per-structure session routing.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetProgram.h"
#include "io/FileLoader.h"
#include "registry/CommandRegistry.h"
#include "registry/StructureRegistries.h"
#include "session/GraphSession.h"
#include "session/HashTableSession.h"
#include "session/QueueSession.h"
#include "session/StackSession.h"
#include "session/TreeSession.h"
#include "ui/Display.h"
#include "ui/Menu.h"
#include "ui/MenuController.h"
#include "ui/StructureMenu.h"


// named container llb = linked list browser
namespace llb
{
    namespace
    {
        /*
         * Purpose: Load the selected dataset into a vector snapshot for structure construction.
         * Design: Reuses FileLoader's fallback policy so every structure receives usable data.
         * Workflow: Load the selected file or built-in fallback records and return a vector snapshot.
         * Data Handoff: Converts the selected path into Target values for a session constructor.
         */
        std::vector<Target> loadTargets(const std::string& dataFilePath)
        {
            TargetList loaded;
            FileLoader::loadTargetsOrFallback(dataFilePath, loaded);
            return loaded.toVector();
        }

        /*
         * Purpose: Report the precise outcome of companion edge loading.
         * Design: Combines parser status with endpoint-resolution counts from GraphSession.
         * Workflow: Print one status message, then report skipped rows or unresolved endpoints.
         * Data Handoff: Converts EdgeLoadResult and GraphSession metadata into user feedback.
         */
        void displayEdgeLoadResult(const EdgeLoadResult& result, const GraphSession& session)
        {
            switch (result.status)
            {
                case EdgeLoadStatus::NotFound:
                    Display::printMessage(
                        "No companion edge file found; graph loaded with nodes only.");
                    break;
                case EdgeLoadStatus::Empty:
                    Display::printMessage(
                        "Companion edge file contains no edge records; graph loaded with nodes only.");
                    break;
                case EdgeLoadStatus::Invalid:
                    Display::printMessage(
                        "Companion edge file contains no valid edge records; skipped " +
                        std::to_string(result.skippedRowCount) + " row(s).");
                    break;
                case EdgeLoadStatus::Loaded:
                    if (result.skippedRowCount > 0)
                    {
                        Display::printMessage(
                            "Loaded edge records with " +
                            std::to_string(result.skippedRowCount) + " invalid row(s) skipped.");
                    }
                    break;
            }

            if (session.unresolvedEdgeCount() > 0)
            {
                Display::printMessage(
                    "Skipped " + std::to_string(session.unresolvedEdgeCount()) +
                    " edge record(s) whose node keys were not found.");
            }
        }
    }

    /*
     * Purpose: Let the user pick a data structure and explore it with the selected dataset.
     * Design: Builds a fresh session per choice and runs the generic MenuController for it.
     * Workflow: Display the structure list, construct the chosen session, and run its operations menu.
     * Data Handoff: Routes the selected data file into each structure session and its menu.
     */
    void StructureMenu::run(const std::string& dataFilePath)
    {
        while (true)
        {
            const std::vector<std::string> labels = {
                "Linked List",
                "Stack",
                "Queue",
                "Binary Search Tree",
                "Graph",
                "Hash Table",
                "Exit"
            };

            const std::size_t choice = Menu::select("Data Structure Menu", labels);

            if (choice == 6)
            {
                Display::printMessage("Exiting program.");
                return;
            }

            switch (choice)
            {
                case 0:
                {
                    TargetProgram program(dataFilePath);
                    program.loadInitialData();
                    MenuController<CommandRegistry, TargetProgram>::run("Target Data Structure Menu", program);
                    break;
                }
                case 1:
                {
                    StackSession session(loadTargets(dataFilePath));
                    MenuController<StackRegistry, StackSession>::run("Stack Menu", session);
                    break;
                }
                case 2:
                {
                    QueueSession session(loadTargets(dataFilePath));
                    MenuController<QueueRegistry, QueueSession>::run("Queue Menu", session);
                    break;
                }
                case 3:
                {
                    TreeSession session(loadTargets(dataFilePath));
                    MenuController<TreeRegistry, TreeSession>::run("Binary Search Tree Menu", session);
                    break;
                }
                case 4:
                {
                    const std::string edgeFile = FileLoader::discoverEdgeFile(dataFilePath);
                    const EdgeLoadResult edgeResult = FileLoader::loadEdges(edgeFile);
                    GraphSession session(loadTargets(dataFilePath), edgeResult.edges);
                    displayEdgeLoadResult(edgeResult, session);
                    MenuController<GraphRegistry, GraphSession>::run("Graph Menu", session);
                    break;
                }
                case 5:
                {
                    HashTableSession session(loadTargets(dataFilePath));
                    MenuController<HashTableRegistry, HashTableSession>::run("Hash Table Menu", session);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}
