/*
 * File: HashTableOperations.cpp
 * Description: Registers the interactive operations available in the hash table menu.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "registry/StructureRegistries.h"
#include "session/HashTableSession.h"


using HashOp = pds::Operation<pds::HashTableSession>;

LLB_REGISTER_OPERATION(pds::HashTableRegistry::instance(),
    HashOp{1, "Show buckets", [](pds::HashTableSession& session) { session.showBuckets(); }})

LLB_REGISTER_OPERATION(pds::HashTableRegistry::instance(),
    HashOp{2, "Show load factor", [](pds::HashTableSession& session) { session.showLoadFactor(); }})

LLB_REGISTER_OPERATION(pds::HashTableRegistry::instance(),
    HashOp{3, "Insert or update an entry", [](pds::HashTableSession& session) { session.insertFromUser(); }})

LLB_REGISTER_OPERATION(pds::HashTableRegistry::instance(),
    HashOp{4, "Find by key", [](pds::HashTableSession& session) { session.findFromUser(); }})

LLB_REGISTER_OPERATION(pds::HashTableRegistry::instance(),
    HashOp{5, "Remove by key", [](pds::HashTableSession& session) { session.eraseFromUser(); }})
