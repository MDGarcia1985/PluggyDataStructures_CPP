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

LLB_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{1, "Count words from typed text", [](pds::MapSession& session) { session.countWordsFromUser(); }})

LLB_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{2, "Count words from loaded target data", [](pds::MapSession& session) { session.countWordsFromLoadedTargets(); }})

LLB_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{3, "Show word frequencies", [](pds::MapSession& session) { session.showWordFrequencies(); }})

LLB_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{4, "Show most frequent word(s)", [](pds::MapSession& session) { session.showMostFrequentWords(); }})

LLB_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{5, "Show word count summary", [](pds::MapSession& session) { session.showSummary(); }})

LLB_REGISTER_OPERATION(pds::MapRegistry::instance(),
    MapOp{6, "Clear word counts", [](pds::MapSession& session) { session.clear(); }})
