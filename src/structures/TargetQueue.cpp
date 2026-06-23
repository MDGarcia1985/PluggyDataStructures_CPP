/*
 * File: TargetQueue.cpp
 * Description: Implements first-in-first-out Target operations over TargetList.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "structures/TargetQueue.h"
#include "ui/Display.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Add a target to the back of the queue.
     * Design: Reuses TargetList storage so queue behavior is still backed by linked-list nodes.
     * Workflow: Append the target to the list, making the back of the list the newest queue entry.
     * Data Handoff: Receives a Target from the caller and stores a copy in queue_.
     */
    void TargetQueue::enqueue(const Target& target)
    {
        queue_.addBack(target);
    }

    /*
     * Purpose: Remove and return the oldest queued target.
     * Design: Treats the list head as the queue front and removes it in constant time.
     * Workflow: Reject empty queues, copy the front Target, remove the front node, and return the copy.
     * Data Handoff: Returns the removed Target to the caller or throws if the queue is empty.
     */
    Target TargetQueue::dequeue()
    {
        if (isEmpty())
        {
            throw std::runtime_error("Cannot dequeue from an empty target queue.");
        }

        Target front = *queue_.front();
        queue_.removeFront();
        return front;
    }

    /*
     * Purpose: Read the oldest queued target without removing it.
     * Design: Treats the list head as the queue front for a constant-time peek.
     * Workflow: Reject empty queues, then return a copy of the front Target.
     * Data Handoff: Returns the front Target to the caller or throws if the queue is empty.
     */
    Target TargetQueue::peek() const
    {
        if (isEmpty())
        {
            throw std::runtime_error("Cannot peek an empty target queue.");
        }

        return *queue_.front();
    }

    /*
     * Purpose: Report whether the queue contains no targets.
     * Design: Delegates to TargetList so size tracking stays in one place.
     * Workflow: Ask queue_ whether it is empty.
     * Data Handoff: Returns a boolean to callers before dequeue operations.
     */
    bool TargetQueue::isEmpty() const
    {
        return queue_.isEmpty();
    }

    /*
     * Purpose: Report how many targets the queue holds.
     * Design: Delegates to TargetList so size tracking stays in one place.
     * Workflow: Return the backing list size.
     * Data Handoff: Gives callers the current queue length.
     */
    std::size_t TargetQueue::size() const
    {
        return queue_.size();
    }

    /*
     * Purpose: Display the current queue contents.
     * Design: Uses Display so output formatting stays outside the data structure.
     * Workflow: Convert the queue list to a vector and pass it to Display.
     * Data Handoff: Sends a read-only snapshot of queue_ to the display layer.
     */
    void TargetQueue::display() const
    {
        Display::printTargetList(queue_.toVector());
    }
}
