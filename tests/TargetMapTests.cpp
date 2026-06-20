/*
 * File: TargetMapTests.cpp
 * Description: Tests normalized map word counts, ties, dataset fields, and registry wiring.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "TestHarness.h"

#include "core/TargetMap.h"
#include "registry/StructureRegistries.h"
#include "session/MapSession.h"


using namespace llbtest;

/*
 * Purpose: Verify case folding, punctuation splitting, totals, and alphabetical map storage.
 * Design: Uses the acceptance example as a focused end-to-end core-structure test.
 * Workflow: Count sample text and assert each aggregate and representative frequency.
 * Data Handoff: Sends text into TargetMap and reads its public analysis results.
 */
LLB_TEST(testMapCountsNormalizedText)
{
    llb::TargetMap map;
    map.countText("Hello hello world! This world is a hello-world test.");

    expectEqual(static_cast<std::size_t>(map.totalWords()), 10, "Map counts every word occurrence.");
    expectEqual(map.uniqueWords(), 6, "Map counts distinct normalized words.");
    expectEqual(static_cast<std::size_t>(map.frequencies().at("hello")), 3, "Map folds case for hello.");
    expectEqual(static_cast<std::size_t>(map.frequencies().at("world")), 3, "Map splits punctuation compounds.");
    expectEqual(static_cast<std::size_t>(map.maxFrequency()), 3, "Map reports the maximum frequency.");

    auto entry = map.frequencies().begin();
    expectEqual(entry->first, "a", "Map iteration begins with the alphabetically first word.");
}

/*
 * Purpose: Verify all words tied for the maximum frequency are returned alphabetically.
 * Design: Exercises the tie path with two equally frequent words and one lower-frequency word.
 * Workflow: Count tie-producing text, collect winners, and assert their values and order.
 * Data Handoff: Reads a vector of winning keys produced from the ordered map.
 */
LLB_TEST(testMapMostFrequentTies)
{
    llb::TargetMap map;
    map.countText("beta alpha beta alpha gamma");

    const std::vector<std::string> words = map.mostFrequentWords();
    expectEqual(words.size(), 2, "Map returns every tied most-frequent word.");
    expectEqual(words[0], "alpha", "Tied words retain alphabetical order.");
    expectEqual(words[1], "beta", "Tied words include the second winner.");
}

/*
 * Purpose: Verify dataset analysis includes both fields and replaces previous counts.
 * Design: Starts with typed text, then runs the dedicated Target-field analysis path.
 * Workflow: Seed counts, analyze two Targets, and assert only field-derived words remain.
 * Data Handoff: Sends a Target vector into TargetMap and reads the replacement frequencies.
 */
LLB_TEST(testMapCountsTargetFields)
{
    llb::TargetMap map;
    map.countText("obsolete");

    const std::vector<llb::Target> targets = {
        llb::Target("Red Apple", "Fresh"),
        llb::Target("Green apple", "Fresh fruit")
    };
    map.countTargetFields(targets);

    expect(map.frequencies().count("obsolete") == 0, "Dataset counting replaces previous analysis.");
    expectEqual(static_cast<std::size_t>(map.totalWords()), 7, "Map counts words from both target fields.");
    expectEqual(static_cast<std::size_t>(map.frequencies().at("apple")), 2, "Repeated field words are combined.");
    expectEqual(static_cast<std::size_t>(map.frequencies().at("fresh")), 2, "Second fields are analyzed.");
}

/*
 * Purpose: Verify clear behavior and map operation registration.
 * Design: Checks core reset semantics alongside first-class structure registry integration.
 * Workflow: Count and clear text, construct a session, then inspect registered operations.
 * Data Handoff: Reads TargetMap state and MapRegistry operation metadata.
 */
LLB_TEST(testMapClearSessionAndRegistry)
{
    llb::TargetMap map;
    map.countText("one two two");
    map.clear();

    expect(map.isEmpty(), "Clear removes every map entry.");
    expectEqual(static_cast<std::size_t>(map.totalWords()), 0, "Clear resets the total count.");

    llb::MapSession session({llb::Target("Alpha", "Beta")});
    session.countWordsFromLoadedTargets();
    expectEqual(static_cast<std::size_t>(session.map().totalWords()), 2, "Map session analyzes loaded targets.");

    const auto operations = llb::MapRegistry::instance().operations();
    expectEqual(operations.size(), 7, "Map registry exposes six operations and Back.");
    expect(operations.back().isExit, "Map registry keeps Back last.");
}
