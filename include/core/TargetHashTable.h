/*
 * File: TargetHashTable.h
 * Description: Declares a separate-chaining hash table of Target records keyed by the first field.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/Target.h"
#include "core/TargetList.h"


// named container llb = linked list browser
namespace llb
{
    struct HashBucketView
    {
        std::size_t index;
        std::vector<Target> entries;
    };

    class TargetHashTable
    {
    private:
        std::vector<TargetList> buckets_;
        std::size_t count_;

        static std::string normalizeKey(const std::string& key);
        std::size_t bucketIndexFor(const std::string& key) const;
        void rehash(std::size_t newBucketCount);

    public:
        explicit TargetHashTable(std::size_t initialBuckets = 8);

        void insert(const Target& target);
        bool find(const std::string& key, Target& found) const;
        bool erase(const std::string& key);

        bool isEmpty() const;
        std::size_t size() const;
        std::size_t bucketCount() const;
        double loadFactor() const;
        std::vector<HashBucketView> bucketViews() const;
    };
}
