/*
 * File: StackSession.cpp
 * Description: Implements interactive stack operations over a TargetStack.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "session/StackSession.h"
#include "ui/Display.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Build a stack session preloaded from the selected dataset.
     * Design: Pushes each loaded Target so the dataset order becomes the initial stack order.
     * Workflow: Iterate the supplied Targets and push each onto the stack.
     * Data Handoff: Receives a vector snapshot and fills the backing TargetStack.
     */
    StackSession::StackSession(const std::vector<Target>& items)
        : stack_()
    {
        for (const Target& item : items)
        {
            stack_.push(item);
        }
    }

    /*
     * Purpose: Show the current stack contents.
     * Design: Delegates rendering to the stack's display helper.
     * Workflow: Ask the stack to print its contents from top to bottom.
     * Data Handoff: Sends the backing stack snapshot to the display layer.
     */
    void StackSession::display() const
    {
        stack_.display();
    }

    /*
     * Purpose: Push a new Target collected from the user.
     * Design: Keeps input handling in the session while the stack owns storage.
     * Workflow: Prompt for two fields, reject blanks, and push the new Target.
     * Data Handoff: Moves console input into a Target and onto the stack.
     */
    void StackSession::pushFromUser()
    {
        std::string fieldOne;
        std::string fieldTwo;

        std::cout << "\nEnter first field: ";
        std::getline(std::cin, fieldOne);
        std::cout << "Enter second field: ";
        std::getline(std::cin, fieldTwo);

        if (fieldOne.empty() || fieldTwo.empty())
        {
            Display::printMessage("Both target fields are required.");
            return;
        }

        stack_.push(Target(fieldOne, fieldTwo));
        Display::printMessage("Pushed target onto the stack.");
    }

    /*
     * Purpose: Pop the top Target and show it.
     * Design: Guards the empty case so popping never throws to the menu loop.
     * Workflow: Report empty stacks, otherwise pop and display the removed Target.
     * Data Handoff: Moves the removed Target from the stack to the display layer.
     */
    void StackSession::popAndShow()
    {
        if (stack_.isEmpty())
        {
            Display::printMessage("The stack is empty.");
            return;
        }

        const Target popped = stack_.pop();
        Display::printMessage("Popped: " + popped.toDisplayString());
    }

    /*
     * Purpose: Show the top Target without removing it.
     * Design: Guards the empty case so peeking never throws to the menu loop.
     * Workflow: Report empty stacks, otherwise display the top Target.
     * Data Handoff: Reads the top Target and sends it to the display layer.
     */
    void StackSession::peekAndShow() const
    {
        if (stack_.isEmpty())
        {
            Display::printMessage("The stack is empty.");
            return;
        }

        Display::printMessage("Top: " + stack_.peek().toDisplayString());
    }

    /*
     * Purpose: Provide mutable access to the backing stack.
     * Design: Exposes the stack for tests and advanced operations.
     * Workflow: Return stack_ by reference.
     * Data Handoff: Gives callers direct access to the session's TargetStack.
     */
    TargetStack& StackSession::stack()
    {
        return stack_;
    }

    /*
     * Purpose: Provide read-only access to the backing stack.
     * Design: Supports const sessions without allowing mutation.
     * Workflow: Return stack_ by const reference.
     * Data Handoff: Gives callers read access to the session's TargetStack.
     */
    const TargetStack& StackSession::stack() const
    {
        return stack_;
    }
}
