/*
 * File: TargetHashTableTests.cpp
 * Description: Tests separate-chaining insertion, lookup, erase, resizing, and registry wiring.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "TestHarness.h"

#include "structures/TargetHashTable.h"
#include "registry/StructureRegistries.h"
#include "session/HashTableSession.h"


using namespace pdstest;

/*
 * Purpose: Verify insertion, lookup, and removal in separate-chaining storage.
 * Design: Exercises successful and missing-key paths with representative entries.
 * Workflow: Insert records, find keys case-insensitively, erase one, and assert state changes.
 * Data Handoff: Sends Targets into the table and reads lookup results and size metrics.
 */
PDS_TEST(testHashInsertFindErase)
{
    pds::TargetHashTable table;
    table.insert(pds::Target("Alice", "Engineer"));
    table.insert(pds::Target("Bob", "Designer"));
    table.insert(pds::Target("Carol", "Manager"));

    expectEqual(table.size(), 3, "Hash table counts inserted entries.");

    pds::Target found;
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
PDS_TEST(testHashUpdateExistingKey)
{
    pds::TargetHashTable table;
    table.insert(pds::Target("Key", "first"));
    table.insert(pds::Target("Key", "second"));

    expectEqual(table.size(), 1, "Re-inserting a key updates rather than duplicates.");

    pds::Target found;
    expect(table.find("Key", found), "Updated key is still found.");
    expectEqual(found.fieldTwo(), "second", "Hash table keeps the latest value for a key.");
}

/*
 * Purpose: Verify resizing preserves entries and the configured load-factor bound.
 * Design: Inserts enough distinct keys to exceed the initial bucket capacity.
 * Workflow: Fill the table, assert growth and load factor, then find a retained entry.
 * Data Handoff: Sends generated Targets through rehashing and reads one result afterward.
 */
PDS_TEST(testHashResizing)
{
    pds::TargetHashTable table(8);
    expectEqual(table.bucketCount(), 8, "Hash table starts with the requested bucket count.");

    for (int index = 0; index < 12; ++index)
    {
        table.insert(pds::Target("Key" + std::to_string(index), std::to_string(index)));
    }

    expectEqual(table.size(), 12, "Hash table stores every distinct key.");
    expect(table.bucketCount() > 8, "Hash table grows once the load factor is exceeded.");
    expect(table.loadFactor() <= 0.75, "Hash table keeps the load factor at or below the threshold.");

    pds::Target found;
    expect(table.find("Key7", found), "Entries remain findable after resizing.");
    expectEqual(found.fieldTwo(), "7", "Resized table preserves stored values.");
}

PDS_TEST(testHashStrategySwitchPreservesEntries)
{
    pds::TargetHashTable table(4);
    table.insert(pds::Target("Alpha", "1"));
    table.insert(pds::Target("Beta", "2"));
    table.insert(pds::Target("Gamma", "3"));

    table.useStrategy(pds::HashStrategy::LinearProbing);
    expect(table.strategy() == pds::HashStrategy::LinearProbing, "Hash table switches to linear probing.");
    expectEqual(table.size(), static_cast<std::size_t>(3), "Switching to probing preserves the entry count.");

    pds::Target found;
    expect(table.find("alpha", found), "Probing strategy finds migrated entries.");
    expectEqual(found.fieldTwo(), "1", "Migrated probing entry keeps its value.");

    table.insert(pds::Target("Delta", "4"));
    table.useStrategy(pds::HashStrategy::SeparateChaining);
    expect(table.strategy() == pds::HashStrategy::SeparateChaining, "Hash table switches back to separate chaining.");
    expectEqual(table.size(), static_cast<std::size_t>(4), "Switching back preserves all entries.");
    expect(table.find("Delta", found), "Chaining strategy finds entries added while probing was active.");
}

PDS_TEST(testHashStrategySwitchKeepsUpdatedValues)
{
    pds::TargetHashTable table(4);
    table.insert(pds::Target("Key", "first"));
    table.useStrategy(pds::HashStrategy::LinearProbing);
    table.insert(pds::Target("KEY", "second"));
    table.useStrategy(pds::HashStrategy::SeparateChaining);

    pds::Target found;
    expect(table.find("key", found), "Strategy migration preserves updated normalized keys.");
    expectEqual(found.fieldTwo(), "second", "Strategy migration keeps the latest value for duplicate keys.");
    expectEqual(table.size(), static_cast<std::size_t>(1), "Strategy migration does not duplicate normalized keys.");
}

/*
 * Purpose: Verify session preload behavior and hash operation registration.
 * Design: Covers integration between loaded Targets, TargetHashTable, and HashTableRegistry.
 * Workflow: Construct a session, inspect its table, then validate registry ordering.
 * Data Handoff: Routes a Target snapshot into the session and reads table and registry state.
 */
PDS_TEST(testHashSessionAndRegistry)
{
    std::vector<pds::Target> items;
    items.push_back(pds::Target("Alice", "Engineer"));
    items.push_back(pds::Target("Bob", "Designer"));

    pds::HashTableSession session(items);
    expectEqual(session.table().size(), 2, "Hash session loads all items.");

    pds::Target found;
    expect(session.table().find("Alice", found), "Hash session entries are findable.");

    const auto operations = pds::HashTableRegistry::instance().operations();
    expect(operations.size() >= 2, "Hash registry holds registered operations.");
    expect(operations.back().isExit, "Hash registry keeps Exit last.");
}
