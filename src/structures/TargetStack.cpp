/*
 * File: TargetStack.cpp
 * Description: Implements last-in-first-out Target operations over TargetList.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetStack.h"
#include "ui/Display.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    /*
     * Purpose: Add a target to the top of the stack.
     * Design: Reuses TargetList storage so stack behavior is still backed by linked-list nodes.
     * Workflow: Append the target to the list, making the back of the list the stack top.
     * Data Handoff: Receives a Target from the caller and stores a copy in stack_.
     */
    void TargetStack::push(const Target& target)
    {
        stack_.addBack(target);
    }

    /*
     * Purpose: Remove and return the most recently pushed target.
     * Design: Treats the list tail as the stack top and removes it in constant time.
     * Workflow: Reject empty stacks, copy the tail Target, remove the tail node, and return the copy.
     * Data Handoff: Returns the removed Target to the caller or throws if the stack is empty.
     */
    Target TargetStack::pop()
    {
        if (isEmpty())
        {
            throw std::runtime_error("Cannot pop from an empty target stack.");
        }

        Target top = *stack_.back();
        stack_.removeBack();
        return top;
    }

    /*
     * Purpose: Read the most recently pushed target without removing it.
     * Design: Treats the list tail as the stack top for a constant-time peek.
     * Workflow: Reject empty stacks, then return a copy of the tail Target.
     * Data Handoff: Returns the top Target to the caller or throws if the stack is empty.
     */
    Target TargetStack::peek() const
    {
        if (isEmpty())
        {
            throw std::runtime_error("Cannot peek an empty target stack.");
        }

        return *stack_.back();
    }

    /*
     * Purpose: Report whether the stack contains no targets.
     * Design: Delegates to TargetList so size tracking stays in one place.
     * Workflow: Ask stack_ whether it is empty.
     * Data Handoff: Returns a boolean to callers before pop operations.
     */
    bool TargetStack::isEmpty() const
    {
        return stack_.isEmpty();
    }

    /*
     * Purpose: Report how many targets the stack holds.
     * Design: Delegates to TargetList so size tracking stays in one place.
     * Workflow: Return the backing list size.
     * Data Handoff: Gives callers the current stack depth.
     */
    std::size_t TargetStack::size() const
    {
        return stack_.size();
    }

    /*
     * Purpose: Display the current stack contents.
     * Design: Uses Display so output formatting stays outside the data structure.
     * Workflow: Convert the stack list to a vector and pass it to Display.
     * Data Handoff: Sends a read-only snapshot of stack_ to the display layer.
     */
    void TargetStack::display() const
    {
        Display::printTargetList(stack_.toVector());
    }
}
