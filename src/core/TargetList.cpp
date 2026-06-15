/*
 * File: TargetList.cpp
 * Description: Implements linked-list node management, navigation, search, copy, and move behavior.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetList.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Build one linked-list node around a Target value.
     * Design: Each node owns its Target data and stores links to the neighboring nodes.
     * Workflow: Copy the provided Target and start with no next or previous neighbors.
     * Data Handoff: Receives a Target from TargetList::addBack and stores it in the list chain.
     */
    TargetList::Node::Node(const Target& target)
        : data(target), next(nullptr), previous(nullptr)
    {
    }

    /*
     * Purpose: Create an empty Target list.
     * Design: Uses head, tail, and current pointers so the list can navigate both directions.
     * Workflow: Start all pointers at nullptr and the count at zero.
     * Data Handoff: Produces a ready-to-fill list for FileLoader, TargetProgram, or tests.
     */
    TargetList::TargetList()
        : head_(nullptr), tail_(nullptr), current_(nullptr), count_(0)
    {
    }

    /*
     * Purpose: Create a deep copy of another TargetList.
     * Design: Copies nodes instead of sharing pointers so the two lists can change independently.
     * Workflow: Copy into a temporary list, then swap that complete copy into this object.
     * Data Handoff: Receives another list and produces this list with the same Target values.
     */
    TargetList::TargetList(const TargetList& other)
        : head_(nullptr), tail_(nullptr), current_(nullptr), count_(0)
    {
        TargetList temporaryCopy;
        temporaryCopy.copyFrom(other);
        swap(temporaryCopy);
    }

    /*
     * Purpose: Move another TargetList into this object without copying each node.
     * Design: Transfers raw node pointers, then empties the source so nodes are not deleted twice.
     * Workflow: Take head, tail, current, and count from other, then reset other to an empty list.
     * Data Handoff: Receives ownership of another list's node chain.
     */
    TargetList::TargetList(TargetList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), current_(other.current_), count_(other.count_)
    {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.current_ = nullptr;
        other.count_ = 0;
    }

    /*
     * Purpose: Replace this list with a deep copy of another list.
     * Design: Uses copy-and-swap so this object stays unchanged if copying fails.
     * Workflow: Copy the other list into a temporary object, then swap the internals.
     * Data Handoff: Receives values from other and returns this list by reference for assignment chaining.
     */
    TargetList& TargetList::operator=(const TargetList& other)
    {
        if (this != &other)
        {
            TargetList temporaryCopy(other);
            swap(temporaryCopy);
        }

        return *this;
    }

    /*
     * Purpose: Replace this list by taking ownership of another list's nodes.
     * Design: Clears current nodes first, then transfers pointers from the source.
     * Workflow: Guard against self-assignment, delete existing nodes, move pointers, and empty the source.
     * Data Handoff: Receives node ownership from other and returns this list by reference.
     */
    TargetList& TargetList::operator=(TargetList&& other) noexcept
    {
        if (this != &other)
        {
            clear();

            head_ = other.head_;
            tail_ = other.tail_;
            current_ = other.current_;
            count_ = other.count_;

            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.current_ = nullptr;
            other.count_ = 0;
        }

        return *this;
    }

    /*
     * Purpose: Release all dynamic memory owned by the list.
     * Design: Delegates cleanup to clear() so destructor behavior matches manual list clearing.
     * Workflow: Walk every node and delete it through clear().
     * Data Handoff: Gives memory back to the program when the list goes out of scope.
     */
    TargetList::~TargetList()
    {
        clear();
    }

    /*
     * Purpose: Remove every Target from the list.
     * Design: Walks forward through raw pointers and deletes each node exactly once.
     * Workflow: Save the next node before deleting the current node, then reset list state.
     * Data Handoff: Consumes the current node chain and leaves an empty list behind.
     */
    void TargetList::clear()
    {
        Node* walker = head_;

        while (walker != nullptr)
        {
            Node* nodeToDelete = walker;
            walker = walker->next;
            delete nodeToDelete;
        }

        head_ = nullptr;
        tail_ = nullptr;
        current_ = nullptr;
        count_ = 0;
    }

    /*
     * Purpose: Report whether the list has no Targets.
     * Design: Uses the maintained count instead of walking the list.
     * Workflow: Compare count_ to zero.
     * Data Handoff: Returns a boolean to callers that need empty-list checks before acting.
     */
    bool TargetList::isEmpty() const
    {
        return count_ == 0;
    }

    /*
     * Purpose: Report how many Targets are stored.
     * Design: Returns the cached count for constant-time access.
     * Workflow: Read count_ directly.
     * Data Handoff: Gives callers the current list length without exposing node pointers.
     */
    std::size_t TargetList::size() const
    {
        return count_;
    }

    /*
     * Purpose: Add a Target to the end of the list.
     * Design: Maintains both head and tail pointers so appending does not require a full walk.
     * Workflow: Allocate a node, attach it after the tail, and update current if the list was empty.
     * Data Handoff: Receives a Target value and stores a copy inside a new list node.
     */
    void TargetList::addBack(const Target& target)
    {
        Node* newNode = new Node(target);

        if (isEmpty())
        {
            head_ = newNode;
            tail_ = newNode;
            current_ = newNode;
        }
        else
        {
            tail_->next = newNode;
            newNode->previous = tail_;
            tail_ = newNode;
        }

        ++count_;
    }

    /*
     * Purpose: Remove the Target at a one-based position.
     * Design: Relinks neighboring nodes before deleting the selected node.
     * Workflow: Find the node, update current when needed, reconnect previous and next, then delete.
     * Data Handoff: Receives a user-facing position and returns whether removal succeeded.
     */
    bool TargetList::removeAt(std::size_t position)
    {
        Node* nodeToDelete = nodeAt(position);

        if (nodeToDelete == nullptr)
        {
            return false;
        }

        if (nodeToDelete == current_)
        {
            if (nodeToDelete->next != nullptr)
            {
                current_ = nodeToDelete->next;
            }
            else
            {
                current_ = nodeToDelete->previous;
            }
        }

        if (nodeToDelete->previous != nullptr)
        {
            nodeToDelete->previous->next = nodeToDelete->next;
        }
        else
        {
            head_ = nodeToDelete->next;
        }

        if (nodeToDelete->next != nullptr)
        {
            nodeToDelete->next->previous = nodeToDelete->previous;
        }
        else
        {
            tail_ = nodeToDelete->previous;
        }

        delete nodeToDelete;
        --count_;

        if (count_ == 0)
        {
            current_ = nullptr;
        }

        return true;
    }

    /*
     * Purpose: Read the Target stored at the front (head) of the list.
     * Design: Returns a pointer so nullptr can represent an empty list.
     * Workflow: Return the address of the head node's data when the list is not empty.
     * Data Handoff: Gives queue-style callers O(1) read access to the oldest Target.
     */
    const Target* TargetList::front() const
    {
        return head_ != nullptr ? &head_->data : nullptr;
    }

    /*
     * Purpose: Read the Target stored at the back (tail) of the list.
     * Design: Returns a pointer so nullptr can represent an empty list.
     * Workflow: Return the address of the tail node's data when the list is not empty.
     * Data Handoff: Gives stack-style callers O(1) read access to the newest Target.
     */
    const Target* TargetList::back() const
    {
        return tail_ != nullptr ? &tail_->data : nullptr;
    }

    /*
     * Purpose: Remove the Target at the front (head) of the list in constant time.
     * Design: Reuses removeAt(1), whose lookup returns the head node without walking the chain.
     * Workflow: Reject empty lists, then delegate to removeAt for the first position.
     * Data Handoff: Supports queue dequeue without copying the whole list to a vector.
     */
    bool TargetList::removeFront()
    {
        if (head_ == nullptr)
        {
            return false;
        }

        return removeAt(1);
    }

    /*
     * Purpose: Remove the Target at the back (tail) of the list in constant time.
     * Design: Uses the maintained tail pointer instead of walking to the final node.
     * Workflow: Detach the tail, fix current and head/tail links, delete the node, and update count.
     * Data Handoff: Supports stack pop without copying the whole list to a vector.
     */
    bool TargetList::removeBack()
    {
        if (tail_ == nullptr)
        {
            return false;
        }

        Node* nodeToDelete = tail_;

        if (nodeToDelete == current_)
        {
            current_ = nodeToDelete->previous;
        }

        if (nodeToDelete->previous != nullptr)
        {
            nodeToDelete->previous->next = nullptr;
            tail_ = nodeToDelete->previous;
        }
        else
        {
            head_ = nullptr;
            tail_ = nullptr;
        }

        delete nodeToDelete;
        --count_;

        if (count_ == 0)
        {
            current_ = nullptr;
        }

        return true;
    }

    /*
     * Purpose: Move the current pointer to the next Target.
     * Design: Wraps from tail back to head so navigation can keep cycling.
     * Workflow: Reject empty lists, then advance current_ or wrap to head_.
     * Data Handoff: Updates internal navigation state and returns whether movement occurred.
     */
    bool TargetList::moveForward()
    {
        if (isEmpty())
        {
            return false;
        }

        current_ = (current_ != nullptr && current_->next != nullptr) ? current_->next : head_;
        return true;
    }

    /*
     * Purpose: Move the current pointer to the previous Target.
     * Design: Wraps from head back to tail so reverse navigation can keep cycling.
     * Workflow: Reject empty lists, then move current_ backward or wrap to tail_.
     * Data Handoff: Updates internal navigation state and returns whether movement occurred.
     */
    bool TargetList::moveBackward()
    {
        if (isEmpty())
        {
            return false;
        }

        current_ = (current_ != nullptr && current_->previous != nullptr) ? current_->previous : tail_;
        return true;
    }

    /*
     * Purpose: Select a Target by its one-based list position.
     * Design: Reuses nodeAt() so position validation stays in one helper.
     * Workflow: Find the node for the position and make it the current node when found.
     * Data Handoff: Receives a menu/search position and returns whether selection succeeded.
     */
    bool TargetList::setCurrentToPosition(std::size_t position)
    {
        Node* selectedNode = nodeAt(position);

        if (selectedNode == nullptr)
        {
            return false;
        }

        current_ = selectedNode;
        return true;
    }

    /*
     * Purpose: Read the currently selected Target.
     * Design: Returns a pointer so nullptr can represent no current Target.
     * Workflow: Check current_, then return the address of the node's Target data.
     * Data Handoff: Gives Display or TargetProgram read-only access to the current Target.
     */
    const Target* TargetList::current() const
    {
        if (current_ == nullptr)
        {
            return nullptr;
        }

        return &current_->data;
    }

    /*
     * Purpose: Convert the current node pointer into a one-based position.
     * Design: Walks from head because nodes store links, not numeric indexes.
     * Workflow: Count each node until current_ is found, or return zero if no match exists.
     * Data Handoff: Gives Display a user-friendly position label for the current Target.
     */
    std::size_t TargetList::currentPosition() const
    {
        std::size_t position = 1;
        Node* walker = head_;

        while (walker != nullptr)
        {
            if (walker == current_)
            {
                return position;
            }

            walker = walker->next;
            ++position;
        }

        return 0;
    }

    /*
     * Purpose: Copy all Target values into a vector.
     * Design: Keeps node pointers private while providing a simple container for display and tests.
     * Workflow: Reserve enough space, walk the nodes, and push each Target into the vector.
     * Data Handoff: Returns a std::vector<Target> snapshot to callers.
     */
    std::vector<Target> TargetList::toVector() const
    {
        std::vector<Target> targets;
        targets.reserve(count_);

        Node* walker = head_;
        while (walker != nullptr)
        {
            targets.push_back(walker->data);
            walker = walker->next;
        }

        return targets;
    }

    /*
     * Purpose: Find every Target whose stored fields match a search term.
     * Design: Stores both the one-based position and Target copy so results can be displayed and selected.
     * Workflow: Walk the list, ask each Target if it matches, and collect successful matches.
     * Data Handoff: Returns search result records to TargetProgram and Display.
     */
    std::vector<TargetSearchResult> TargetList::findAll(const std::string& searchTerm) const
    {
        std::vector<TargetSearchResult> results;

        Node* walker = head_;
        std::size_t position = 1;

        while (walker != nullptr)
        {
            if (walker->data.matches(searchTerm))
            {
                results.push_back(TargetSearchResult{position, walker->data});
            }

            walker = walker->next;
            ++position;
        }

        return results;
    }


    /*
     * Purpose: Exchange the internal state of two TargetList objects.
     * Design: Swaps only pointers and count, which is faster than copying nodes.
     * Workflow: Use std::swap on head, tail, current, and count.
     * Data Handoff: Moves ownership metadata between lists without changing the nodes themselves.
     */
    void TargetList::swap(TargetList& other) noexcept
    {
        using std::swap;

        swap(head_, other.head_);
        swap(tail_, other.tail_);
        swap(current_, other.current_);
        swap(count_, other.count_);
    }

    /*
     * Purpose: Locate a node by one-based position.
     * Design: Centralizes position validation for remove and selection operations.
     * Workflow: Reject out-of-range positions, then walk forward until the requested node is reached.
     * Data Handoff: Returns an internal Node pointer to list methods, or nullptr for invalid input.
     */
    TargetList::Node* TargetList::nodeAt(std::size_t position) const
    {
        if (position < 1 || position > count_)
        {
            return nullptr;
        }

        Node* walker = head_;

        for (std::size_t i = 1; i < position; ++i)
        {
            walker = walker->next;
        }

        return walker;
    }

    /*
     * Purpose: Append copies of every node from another list.
     * Design: Uses addBack() so head, tail, current, and count stay consistent.
     * Workflow: Remember the source current position, copy each Target, then restore matching current position.
     * Data Handoff: Receives another list's values and duplicates them into this list.
     */
    void TargetList::copyFrom(const TargetList& other)
    {
        Node* walker = other.head_;
        std::size_t otherCurrentPosition = other.currentPosition();

        while (walker != nullptr)
        {
            addBack(walker->data);
            walker = walker->next;
        }

        if (otherCurrentPosition > 0)
        {
            setCurrentToPosition(otherCurrentPosition);
        }
    }
}

