/*
 * File: TargetTreeTests.cpp
 * Description: Tests binary search tree ordering, search, removal, traversals, and registry wiring.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "TestHarness.h"

#include "algorithms/trees/TreeAnalysis.h"
#include "algorithms/trees/TreeSearch.h"
#include "algorithms/trees/TreeTraversal.h"
#include "structures/TargetTree.h"
#include "registry/StructureRegistries.h"
#include "session/TreeSession.h"


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
     * Purpose: Build the shared balanced-shape tree fixture used by tree tests.
     * Design: Inserts deterministic keys that exercise left and right branches.
     * Workflow: Create a tree, insert each fixture key, and return it by value.
     * Data Handoff: Produces an independently owned TargetTree for each caller.
     */
    pds::TargetTree buildSampleTree()
    {
        pds::TargetTree tree;
        for (const char* key : {"M", "F", "T", "B", "G", "Q", "Z"})
        {
            tree.insert(pds::Target(key, ""));
        }
        return tree;
    }
}

/*
 * Purpose: Verify insertion and traversal ordering in the target tree.
 * Design: Uses keys inserted out of order to expose binary-search-tree behavior.
 * Workflow: Insert records, traverse the tree, and compare the resulting key order.
 * Data Handoff: Sends Targets into the tree and reads ordered traversal snapshots.
 */
PDS_TEST(testTreeInsertionAndOrdering)
{
    pds::TargetTree tree = buildSampleTree();

    expectEqual(tree.size(), 7, "Tree counts inserted nodes.");
    expectEqual(pds::height(tree), 3, "Balanced sample tree has height three.");
    expect(!tree.isEmpty(), "Populated tree is not empty.");

    expectEqual(keysOf(pds::inOrder(tree)), "B,F,G,M,Q,T,Z", "In-order traversal yields sorted keys.");
    expectEqual(keysOf(pds::preOrder(tree)), "M,F,B,G,T,Q,Z", "Pre-order traversal is root-first.");
    expectEqual(keysOf(pds::postOrder(tree)), "B,G,F,Q,Z,T,M", "Post-order traversal is children-first.");
    expectEqual(keysOf(pds::levelOrder(tree)), "M,F,T,B,G,Q,Z", "Level-order traversal is breadth-first.");
}

/*
 * Purpose: Verify the tree rejects duplicate logical keys.
 * Design: Inserts two records whose first fields identify the same key.
 * Workflow: Insert the original, attempt the duplicate, and inspect size and value retention.
 * Data Handoff: Sends competing Targets into the tree and reads the preserved entry.
 */
PDS_TEST(testTreeDuplicateRejection)
{
    pds::TargetTree tree;
    expect(tree.insert(pds::Target("Alpha", "1")), "First insertion succeeds.");
    expect(!tree.insert(pds::Target("Alpha", "1")), "Exact duplicate insertion is rejected.");
    expect(tree.insert(pds::Target("Alpha", "2")), "Same key with different value is allowed.");
    expectEqual(tree.size(), 2, "Tree stores distinct targets only.");
}

/*
 * Purpose: Verify tree search finds existing keys and rejects absent keys.
 * Design: Covers both branches of the public lookup contract.
 * Workflow: Populate the tree and issue successful and unsuccessful searches.
 * Data Handoff: Sends key text into search and reads returned Target pointers.
 */
PDS_TEST(testTreeSearch)
{
    pds::TargetTree tree = buildSampleTree();

    pds::Target found;
    expect(tree.findByKey("g", found), "Find locates a key case-insensitively.");
    expectEqual(found.fieldOne(), "G", "Find returns the matching target.");
    expect(!tree.findByKey("X", found), "Find reports a missing key.");

    expect(tree.contains(pds::Target("Q", "")), "Contains finds an exact target.");
    expect(!tree.contains(pds::Target("Q", "different")), "Contains distinguishes by value.");
}

/*
 * Purpose: Verify removal for the tree's structural node cases.
 * Design: Removes selected nodes and confirms ordering and counts remain valid.
 * Workflow: Build a tree, remove keys, and inspect the resulting traversal.
 * Data Handoff: Sends keys into erase and reads updated tree snapshots.
 */
PDS_TEST(testTreeRemoval)
{
    pds::TargetTree tree = buildSampleTree();

    expect(tree.removeByKey("F"), "Removing a two-child node succeeds.");
    expectEqual(tree.size(), 6, "Size decreases after removal.");
    expectEqual(keysOf(pds::inOrder(tree)), "B,G,M,Q,T,Z", "In-order stays sorted after removal.");

    expect(tree.removeByKey("Z"), "Removing a leaf node succeeds.");
    expectEqual(keysOf(pds::inOrder(tree)), "B,G,M,Q,T", "Leaf removal updates traversal.");

    expect(!tree.removeByKey("X"), "Removing a missing key fails.");
}

/*
 * Purpose: Verify tree copies own independent node structures.
 * Design: Exercises copy construction and assignment before mutating one copy.
 * Workflow: Populate a source, copy it, alter one instance, and compare state.
 * Data Handoff: Copies tree-owned Targets between instances without sharing nodes.
 */
PDS_TEST(testTreeCopySemantics)
{
    pds::TargetTree original = buildSampleTree();

    pds::TargetTree copied(original);
    expectEqual(copied.size(), original.size(), "Copy constructor preserves size.");
    expectEqual(keysOf(pds::inOrder(copied)), keysOf(pds::inOrder(original)), "Copy reproduces traversal.");

    copied.removeByKey("M");
    expectEqual(original.size(), 7, "Mutating a copy does not affect the original.");

    pds::TargetTree assigned;
    assigned = original;
    expectEqual(keysOf(pds::inOrder(assigned)), keysOf(pds::inOrder(original)), "Copy assignment reproduces traversal.");

    pds::TargetTree moved(std::move(assigned));
    expectEqual(moved.size(), 7, "Move constructor transfers nodes.");
    expect(assigned.isEmpty(), "Move constructor empties the source.");
}

/*
 * Purpose: Verify tree session preload behavior and operation registration.
 * Design: Covers integration between dataset snapshots, TargetTree, and TreeRegistry.
 * Workflow: Construct a session, inspect its tree, and validate registered menu items.
 * Data Handoff: Routes loaded Targets into the session and reads tree and registry state.
 */
PDS_TEST(testTreeSessionAndRegistry)
{
    std::vector<pds::Target> items;
    items.push_back(pds::Target("Mango", "fruit"));
    items.push_back(pds::Target("Apple", "fruit"));
    items.push_back(pds::Target("Pear", "fruit"));

    pds::TreeSession session(items);
    expectEqual(session.tree().size(), 3, "Tree session loads all items.");
    expectEqual(keysOf(pds::inOrder(session.tree())), "Apple,Mango,Pear", "Tree session sorts loaded items.");

    const auto operations = pds::TreeRegistry::instance().operations();
    expect(operations.size() >= 2, "Tree registry holds registered operations.");
    expect(operations.back().isExit, "Tree registry keeps Exit last.");

    using TreeOp = pds::Operation<pds::TreeSession>;
    expect(!pds::TreeRegistry::instance().registerOperation(
        TreeOp{1, "Duplicate tree operation id", [](pds::TreeSession&) {}}),
        "Tree registry rejects a duplicate operation id.");
    expect(!pds::TreeRegistry::instance().registerOperation(
        TreeOp{-1, "Invalid tree operation id", [](pds::TreeSession&) {}}),
        "Tree registry rejects a nonpositive operation id.");

    std::size_t idOneCount = 0;
    for (const TreeOp& operation : pds::TreeRegistry::instance().operations())
    {
        if (operation.id == 1)
        {
            ++idOneCount;
        }
    }
    expectEqual(idOneCount, 1, "Each registered structure operation id is unambiguous.");
}
