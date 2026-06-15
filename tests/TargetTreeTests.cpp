/*
 * File: TargetTreeTests.cpp
 * Description: Tests binary search tree ordering, search, removal, traversals, and registry wiring.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "TestHarness.h"

#include "core/TargetTree.h"
#include "registry/StructureRegistries.h"
#include "session/TreeSession.h"


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

    llb::TargetTree buildSampleTree()
    {
        llb::TargetTree tree;
        for (const char* key : {"M", "F", "T", "B", "G", "Q", "Z"})
        {
            tree.insert(llb::Target(key, ""));
        }
        return tree;
    }
}

LLB_TEST(testTreeInsertionAndOrdering)
{
    llb::TargetTree tree = buildSampleTree();

    expectEqual(tree.size(), 7, "Tree counts inserted nodes.");
    expectEqual(tree.height(), 3, "Balanced sample tree has height three.");
    expect(!tree.isEmpty(), "Populated tree is not empty.");

    expectEqual(keysOf(tree.inOrder()), "B,F,G,M,Q,T,Z", "In-order traversal yields sorted keys.");
    expectEqual(keysOf(tree.preOrder()), "M,F,B,G,T,Q,Z", "Pre-order traversal is root-first.");
    expectEqual(keysOf(tree.postOrder()), "B,G,F,Q,Z,T,M", "Post-order traversal is children-first.");
    expectEqual(keysOf(tree.levelOrder()), "M,F,T,B,G,Q,Z", "Level-order traversal is breadth-first.");
}

LLB_TEST(testTreeDuplicateRejection)
{
    llb::TargetTree tree;
    expect(tree.insert(llb::Target("Alpha", "1")), "First insertion succeeds.");
    expect(!tree.insert(llb::Target("Alpha", "1")), "Exact duplicate insertion is rejected.");
    expect(tree.insert(llb::Target("Alpha", "2")), "Same key with different value is allowed.");
    expectEqual(tree.size(), 2, "Tree stores distinct targets only.");
}

LLB_TEST(testTreeSearch)
{
    llb::TargetTree tree = buildSampleTree();

    llb::Target found;
    expect(tree.findByKey("g", found), "Find locates a key case-insensitively.");
    expectEqual(found.fieldOne(), "G", "Find returns the matching target.");
    expect(!tree.findByKey("X", found), "Find reports a missing key.");

    expect(tree.contains(llb::Target("Q", "")), "Contains finds an exact target.");
    expect(!tree.contains(llb::Target("Q", "different")), "Contains distinguishes by value.");
}

LLB_TEST(testTreeRemoval)
{
    llb::TargetTree tree = buildSampleTree();

    expect(tree.removeByKey("F"), "Removing a two-child node succeeds.");
    expectEqual(tree.size(), 6, "Size decreases after removal.");
    expectEqual(keysOf(tree.inOrder()), "B,G,M,Q,T,Z", "In-order stays sorted after removal.");

    expect(tree.removeByKey("Z"), "Removing a leaf node succeeds.");
    expectEqual(keysOf(tree.inOrder()), "B,G,M,Q,T", "Leaf removal updates traversal.");

    expect(!tree.removeByKey("X"), "Removing a missing key fails.");
}

LLB_TEST(testTreeCopySemantics)
{
    llb::TargetTree original = buildSampleTree();

    llb::TargetTree copied(original);
    expectEqual(copied.size(), original.size(), "Copy constructor preserves size.");
    expectEqual(keysOf(copied.inOrder()), keysOf(original.inOrder()), "Copy reproduces traversal.");

    copied.removeByKey("M");
    expectEqual(original.size(), 7, "Mutating a copy does not affect the original.");

    llb::TargetTree assigned;
    assigned = original;
    expectEqual(keysOf(assigned.inOrder()), keysOf(original.inOrder()), "Copy assignment reproduces traversal.");

    llb::TargetTree moved(std::move(assigned));
    expectEqual(moved.size(), 7, "Move constructor transfers nodes.");
    expect(assigned.isEmpty(), "Move constructor empties the source.");
}

LLB_TEST(testTreeSessionAndRegistry)
{
    std::vector<llb::Target> items;
    items.push_back(llb::Target("Mango", "fruit"));
    items.push_back(llb::Target("Apple", "fruit"));
    items.push_back(llb::Target("Pear", "fruit"));

    llb::TreeSession session(items);
    expectEqual(session.tree().size(), 3, "Tree session loads all items.");
    expectEqual(keysOf(session.tree().inOrder()), "Apple,Mango,Pear", "Tree session sorts loaded items.");

    const auto operations = llb::TreeRegistry::instance().operations();
    expect(operations.size() >= 2, "Tree registry holds registered operations.");
    expect(operations.back().isExit, "Tree registry keeps Exit last.");

    using TreeOp = llb::Operation<llb::TreeSession>;
    expect(!llb::TreeRegistry::instance().registerOperation(
        TreeOp{1, "Duplicate tree operation id", [](llb::TreeSession&) {}}),
        "Tree registry rejects a duplicate operation id.");
    expect(!llb::TreeRegistry::instance().registerOperation(
        TreeOp{-1, "Invalid tree operation id", [](llb::TreeSession&) {}}),
        "Tree registry rejects a nonpositive operation id.");

    std::size_t idOneCount = 0;
    for (const TreeOp& operation : llb::TreeRegistry::instance().operations())
    {
        if (operation.id == 1)
        {
            ++idOneCount;
        }
    }
    expectEqual(idOneCount, 1, "Each registered structure operation id is unambiguous.");
}
