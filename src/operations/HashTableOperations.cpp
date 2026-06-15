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


using HashOp = llb::Operation<llb::HashTableSession>;

LLB_REGISTER_OPERATION(llb::HashTableRegistry::instance(),
    HashOp{1, "Show buckets", [](llb::HashTableSession& session) { session.showBuckets(); }})

LLB_REGISTER_OPERATION(llb::HashTableRegistry::instance(),
    HashOp{2, "Show load factor", [](llb::HashTableSession& session) { session.showLoadFactor(); }})

LLB_REGISTER_OPERATION(llb::HashTableRegistry::instance(),
    HashOp{3, "Insert or update an entry", [](llb::HashTableSession& session) { session.insertFromUser(); }})

LLB_REGISTER_OPERATION(llb::HashTableRegistry::instance(),
    HashOp{4, "Find by key", [](llb::HashTableSession& session) { session.findFromUser(); }})

LLB_REGISTER_OPERATION(llb::HashTableRegistry::instance(),
    HashOp{5, "Remove by key", [](llb::HashTableSession& session) { session.eraseFromUser(); }})
