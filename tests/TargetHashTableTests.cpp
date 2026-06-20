/*
 * File: TargetHashTableTests.cpp
 * Description: Tests separate-chaining insertion, lookup, erase, resizing, and registry wiring.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "TestHarness.h"

#include "core/TargetHashTable.h"
#include "registry/StructureRegistries.h"
#include "session/HashTableSession.h"


using namespace llbtest;

/*
 * Purpose: Verify insertion, lookup, and removal in separate-chaining storage.
 * Design: Exercises successful and missing-key paths with representative entries.
 * Workflow: Insert records, find keys case-insensitively, erase one, and assert state changes.
 * Data Handoff: Sends Targets into the table and reads lookup results and size metrics.
 */
LLB_TEST(testHashInsertFindErase)
{
    llb::TargetHashTable table;
    table.insert(llb::Target("Alice", "Engineer"));
    table.insert(llb::Target("Bob", "Designer"));
    table.insert(llb::Target("Carol", "Manager"));

    expectEqual(table.size(), 3, "Hash table counts inserted entries.");

    llb::Target found;
    expect(table.find("bob", found), "Hash table finds an entry case-insensitively.");
    expectEqual(found.fieldTwo(), "Designer", "Hash table returns the stored value.");
    expect(!table.find("Dave", found), "Hash table reports a missing key.");

    expect(table.erase("Alice"), "Hash table erases an existing key.");
    expectEqual(table.size(), 2, "Erase decreases the entry count.");
    expect(!table.find("Alice", found), "Erased key is no longer found.");
    expect(!table.erase("Alice"), "Erasing a missing key fails.");
}

/*
 * Purpose: Verify reinserting an existing key updates its value without duplication.
 * Design: Uses two records with one logical key and different values.
 * Workflow: Insert both records, inspect size, and retrieve the final stored value.
 * Data Handoff: Passes replacement data through insert and reads it back through find.
 */
LLB_TEST(testHashUpdateExistingKey)
{
    llb::TargetHashTable table;
    table.insert(llb::Target("Key", "first"));
    table.insert(llb::Target("Key", "second"));

    expectEqual(table.size(), 1, "Re-inserting a key updates rather than duplicates.");

    llb::Target found;
    expect(table.find("Key", found), "Updated key is still found.");
    expectEqual(found.fieldTwo(), "second", "Hash table keeps the latest value for a key.");
}

/*
 * Purpose: Verify resizing preserves entries and the configured load-factor bound.
 * Design: Inserts enough distinct keys to exceed the initial bucket capacity.
 * Workflow: Fill the table, assert growth and load factor, then find a retained entry.
 * Data Handoff: Sends generated Targets through rehashing and reads one result afterward.
 */
LLB_TEST(testHashResizing)
{
    llb::TargetHashTable table(8);
    expectEqual(table.bucketCount(), 8, "Hash table starts with the requested bucket count.");

    for (int index = 0; index < 12; ++index)
    {
        table.insert(llb::Target("Key" + std::to_string(index), std::to_string(index)));
    }

    expectEqual(table.size(), 12, "Hash table stores every distinct key.");
    expect(table.bucketCount() > 8, "Hash table grows once the load factor is exceeded.");
    expect(table.loadFactor() <= 0.75, "Hash table keeps the load factor at or below the threshold.");

    llb::Target found;
    expect(table.find("Key7", found), "Entries remain findable after resizing.");
    expectEqual(found.fieldTwo(), "7", "Resized table preserves stored values.");
}

/*
 * Purpose: Verify session preload behavior and hash operation registration.
 * Design: Covers integration between loaded Targets, TargetHashTable, and HashTableRegistry.
 * Workflow: Construct a session, inspect its table, then validate registry ordering.
 * Data Handoff: Routes a Target snapshot into the session and reads table and registry state.
 */
LLB_TEST(testHashSessionAndRegistry)
{
    std::vector<llb::Target> items;
    items.push_back(llb::Target("Alice", "Engineer"));
    items.push_back(llb::Target("Bob", "Designer"));

    llb::HashTableSession session(items);
    expectEqual(session.table().size(), 2, "Hash session loads all items.");

    llb::Target found;
    expect(session.table().find("Alice", found), "Hash session entries are findable.");

    const auto operations = llb::HashTableRegistry::instance().operations();
    expect(operations.size() >= 2, "Hash registry holds registered operations.");
    expect(operations.back().isExit, "Hash registry keeps Exit last.");
}
