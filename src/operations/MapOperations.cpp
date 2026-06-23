/*
 * File: MapOperations.cpp
 * Description: Registers the interactive operations available in the word-frequency map menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "registry/StructureRegistries.h"
#include "session/MapSession.h"


using MapOp = pds::Operation<pds::MapSession>;

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{1, "Count words from typed text", [](pds::MapSession& session) { session.countWordsFromUser(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{2, "Count words from loaded target data", [](pds::MapSession& session) { session.countWordsFromLoadedTargets(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{3, "Show word frequencies", [](pds::MapSession& session) { session.showWordFrequencies(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{4, "Show most frequent word(s)", [](pds::MapSession& session) { session.showMostFrequentWords(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{5, "Show word count summary", [](pds::MapSession& session) { session.showSummary(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{6, "Clear word counts", [](pds::MapSession& session) { session.clear(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{7, "Show top N words", [](pds::MapSession& session) { session.showTopNFromUser(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{8, "Show least frequent N words", [](pds::MapSession& session) { session.showLeastFrequentFromUser(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{9, "Show frequency buckets", [](pds::MapSession& session) { session.showFrequencyBuckets(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{10, "Search for a word", [](pds::MapSession& session) { session.searchWordFromUser(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{11, "List words starting with prefix", [](pds::MapSession& session) { session.showWordsStartingWithFromUser(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{12, "Show frequency ranking", [](pds::MapSession& session) { session.showFrequencyRanking(); }})

PDS_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{13, "Show alphabetical ranking", [](pds::MapSession& session) { session.showAlphabeticalRanking(); }})
