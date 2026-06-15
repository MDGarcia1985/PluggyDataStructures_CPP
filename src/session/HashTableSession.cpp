/*
 * File: HashTableSession.cpp
 * Description: Implements interactive hash table operations over a TargetHashTable.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "session/HashTableSession.h"
#include "ui/Display.h"

#include <iomanip>


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Build a hash table session preloaded from the selected dataset.
     * Design: Inserts each loaded Target, keyed by its first field, resizing as needed.
     * Workflow: Iterate the supplied Targets and insert each one.
     * Data Handoff: Receives a vector snapshot and fills the backing TargetHashTable.
     */
    HashTableSession::HashTableSession(const std::vector<Target>& items)
        : table_()
    {
        for (const Target& item : items)
        {
            table_.insert(item);
        }
    }

    /*
     * Purpose: Insert or update a Target collected from the user.
     * Design: Keeps input handling in the session while the table owns hashing.
     * Workflow: Prompt for two fields, reject a blank key, and insert the Target.
     * Data Handoff: Moves console input into a Target and into the hash table.
     */
    void HashTableSession::insertFromUser()
    {
        std::string fieldOne;
        std::string fieldTwo;

        std::cout << "\nEnter key (first field): ";
        std::getline(std::cin, fieldOne);
        std::cout << "Enter value (second field): ";
        std::getline(std::cin, fieldTwo);

        if (fieldOne.empty())
        {
            Display::printMessage("The key is required.");
            return;
        }

        table_.insert(Target(fieldOne, fieldTwo));
        Display::printMessage("Inserted or updated the entry.");
    }

    /*
     * Purpose: Find a Target by its key.
     * Design: Uses the hash lookup so only one short bucket chain is scanned.
     * Workflow: Prompt for a key, search the table, and display the match or a not-found message.
     * Data Handoff: Moves user text into the table lookup and shows the result.
     */
    void HashTableSession::findFromUser() const
    {
        std::string key;
        std::cout << "\nEnter key to find: ";
        std::getline(std::cin, key);

        if (key.empty())
        {
            Display::printMessage("Search key cannot be empty.");
            return;
        }

        Target found;
        if (table_.find(key, found))
        {
            Display::printMessage("Found: " + found.toDisplayString());
        }
        else
        {
            Display::printMessage("No matching entry found.");
        }
    }

    /*
     * Purpose: Remove a Target by its key.
     * Design: Uses the hash lookup so only the owning bucket chain is touched.
     * Workflow: Prompt for a key, erase the matching entry, and report the outcome.
     * Data Handoff: Moves user text into the table's key-based removal.
     */
    void HashTableSession::eraseFromUser()
    {
        std::string key;
        std::cout << "\nEnter key to remove: ";
        std::getline(std::cin, key);

        if (key.empty())
        {
            Display::printMessage("Removal key cannot be empty.");
            return;
        }

        if (table_.erase(key))
        {
            Display::printMessage("Removed the matching entry.");
        }
        else
        {
            Display::printMessage("No matching entry found.");
        }
    }

    /*
     * Purpose: Show the contents of every bucket.
     * Design: Reveals the chain distribution so collisions and spread are visible.
     * Workflow: Print each bucket index and the display text of its chained entries.
     * Data Handoff: Reads per-bucket snapshots and writes them to the console.
     */
    void HashTableSession::showBuckets() const
    {
        std::cout << "\nHash table buckets (" << table_.size() << " entries in "
            << table_.bucketCount() << " buckets):\n";

        for (const HashBucketView& view : table_.bucketViews())
        {
            std::cout << "  [" << view.index << "]";
            if (view.entries.empty())
            {
                std::cout << " -";
            }
            else
            {
                for (const Target& entry : view.entries)
                {
                    std::cout << " {" << entry.toDisplayString() << '}';
                }
            }
            std::cout << '\n';
        }
    }

    /*
     * Purpose: Report the current load factor.
     * Design: Surfaces the entries-per-bucket ratio used to trigger resizing.
     * Workflow: Read the load factor and display it with fixed precision.
     * Data Handoff: Sends the table metric to the console.
     */
    void HashTableSession::showLoadFactor() const
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(3) << table_.loadFactor();
        Display::printMessage("Load factor: " + stream.str() +
            " (" + std::to_string(table_.size()) + " entries / " +
            std::to_string(table_.bucketCount()) + " buckets)");
    }

    /*
     * Purpose: Provide mutable access to the backing table.
     * Design: Exposes the table for tests and advanced operations.
     * Workflow: Return table_ by reference.
     * Data Handoff: Gives callers direct access to the session's TargetHashTable.
     */
    TargetHashTable& HashTableSession::table()
    {
        return table_;
    }

    /*
     * Purpose: Provide read-only access to the backing table.
     * Design: Supports const sessions without allowing mutation.
     * Workflow: Return table_ by const reference.
     * Data Handoff: Gives callers read access to the session's TargetHashTable.
     */
    const TargetHashTable& HashTableSession::table() const
    {
        return table_;
    }
}
