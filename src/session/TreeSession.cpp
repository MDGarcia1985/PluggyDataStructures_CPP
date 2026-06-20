/*
 * File: TreeSession.cpp
 * Description: Implements interactive binary search tree operations over a TargetTree.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "session/TreeSession.h"
#include "ui/Display.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Build a tree session preloaded from the selected dataset.
     * Design: Inserts each loaded Target so the dataset becomes a searchable BST.
     * Workflow: Iterate the supplied Targets and insert each one.
     * Data Handoff: Receives a vector snapshot and fills the backing TargetTree.
     */
    TreeSession::TreeSession(const std::vector<Target>& items)
        : tree_()
    {
        for (const Target& item : items)
        {
            tree_.insert(item);
        }
    }

    /*
     * Purpose: Insert a new Target collected from the user.
     * Design: Keeps input handling in the session while the tree owns ordering.
     * Workflow: Prompt for two fields, reject blanks, and report whether a node was added.
     * Data Handoff: Moves console input into a Target and into the tree.
     */
    void TreeSession::insertFromUser()
    {
        std::string fieldOne;
        std::string fieldTwo;

        std::cout << "\nEnter first field (sort key): ";
        std::getline(std::cin, fieldOne);
        std::cout << "Enter second field: ";
        std::getline(std::cin, fieldTwo);

        if (fieldOne.empty())
        {
            Display::printMessage("The first field is required.");
            return;
        }

        if (tree_.insert(Target(fieldOne, fieldTwo)))
        {
            Display::printMessage("Inserted target into the tree.");
        }
        else
        {
            Display::printMessage("That exact target already exists in the tree.");
        }
    }

    /*
     * Purpose: Find a Target by its first-field key.
     * Design: Uses the BST key descent so lookups stay efficient on typical data.
     * Workflow: Prompt for a key, search the tree, and display the match or a not-found message.
     * Data Handoff: Moves user text into the tree's key search and shows the result.
     */
    void TreeSession::findFromUser() const
    {
        std::string key;
        std::cout << "\nEnter first field to find: ";
        std::getline(std::cin, key);

        if (key.empty())
        {
            Display::printMessage("Search key cannot be empty.");
            return;
        }

        Target found;
        if (tree_.findByKey(key, found))
        {
            Display::printMessage("Found: " + found.toDisplayString());
        }
        else
        {
            Display::printMessage("No matching target found.");
        }
    }

    /*
     * Purpose: Remove a Target by its first-field key.
     * Design: Reuses the key search so removal targets the same node a lookup would find.
     * Workflow: Prompt for a key, remove the matching node, and report the outcome.
     * Data Handoff: Moves user text into the tree's key-based removal.
     */
    void TreeSession::removeFromUser()
    {
        std::string key;
        std::cout << "\nEnter first field to remove: ";
        std::getline(std::cin, key);

        if (key.empty())
        {
            Display::printMessage("Removal key cannot be empty.");
            return;
        }

        if (tree_.removeByKey(key))
        {
            Display::printMessage("Removed the matching target from the tree.");
        }
        else
        {
            Display::printMessage("No matching target found.");
        }
    }

    /*
     * Purpose: Show the tree in ascending in-order sequence.
     * Design: In-order traversal of a BST yields sorted output.
     * Workflow: Collect the in-order snapshot and display it.
     * Data Handoff: Sends the traversal snapshot to the display layer.
     */
    void TreeSession::showInOrder() const
    {
        Display::printTargets("In-order traversal", tree_.inOrder());
    }

    /*
     * Purpose: Show the tree in pre-order sequence.
     * Design: Pre-order traversal reveals the root-first structure.
     * Workflow: Collect the pre-order snapshot and display it.
     * Data Handoff: Sends the traversal snapshot to the display layer.
     */
    void TreeSession::showPreOrder() const
    {
        Display::printTargets("Pre-order traversal", tree_.preOrder());
    }

    /*
     * Purpose: Show the tree in post-order sequence.
     * Design: Post-order traversal reveals the children-first structure.
     * Workflow: Collect the post-order snapshot and display it.
     * Data Handoff: Sends the traversal snapshot to the display layer.
     */
    void TreeSession::showPostOrder() const
    {
        Display::printTargets("Post-order traversal", tree_.postOrder());
    }

    /*
     * Purpose: Show the tree in breadth-first level order.
     * Design: Level-order traversal visits each depth before the next.
     * Workflow: Collect the level-order snapshot and display it.
     * Data Handoff: Sends the traversal snapshot to the display layer.
     */
    void TreeSession::showLevelOrder() const
    {
        Display::printTargets("Level-order traversal", tree_.levelOrder());
    }

    /*
     * Purpose: Report the current tree height and node count.
     * Design: Surfaces a simple balance indicator for the loaded data.
     * Workflow: Read the height and size, then display them together.
     * Data Handoff: Sends tree metrics to the display layer.
     */
    void TreeSession::showHeight() const
    {
        Display::printMessage("Tree height: " + std::to_string(tree_.height()) +
            ", nodes: " + std::to_string(tree_.size()));
    }

    /*
     * Purpose: Provide mutable access to the backing tree.
     * Design: Exposes the tree for tests and advanced operations.
     * Workflow: Return tree_ by reference.
     * Data Handoff: Gives callers direct access to the session's TargetTree.
     */
    TargetTree& TreeSession::tree()
    {
        return tree_;
    }

    /*
     * Purpose: Provide read-only access to the backing tree.
     * Design: Supports const sessions without allowing mutation.
     * Workflow: Return tree_ by const reference.
     * Data Handoff: Gives callers read access to the session's TargetTree.
     */
    const TargetTree& TreeSession::tree() const
    {
        return tree_;
    }
}
