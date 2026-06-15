/*
 * File: QueueSession.cpp
 * Description: Implements interactive queue operations over a TargetQueue.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "session/QueueSession.h"
#include "ui/Display.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Build a queue session preloaded from the selected dataset.
     * Design: Enqueues each loaded Target so dataset order becomes the initial queue order.
     * Workflow: Iterate the supplied Targets and enqueue each one.
     * Data Handoff: Receives a vector snapshot and fills the backing TargetQueue.
     */
    QueueSession::QueueSession(const std::vector<Target>& items)
        : queue_()
    {
        for (const Target& item : items)
        {
            queue_.enqueue(item);
        }
    }

    /*
     * Purpose: Show the current queue contents.
     * Design: Delegates rendering to the queue's display helper.
     * Workflow: Ask the queue to print its contents from front to back.
     * Data Handoff: Sends the backing queue snapshot to the display layer.
     */
    void QueueSession::display() const
    {
        queue_.display();
    }

    /*
     * Purpose: Enqueue a new Target collected from the user.
     * Design: Keeps input handling in the session while the queue owns storage.
     * Workflow: Prompt for two fields, reject blanks, and enqueue the new Target.
     * Data Handoff: Moves console input into a Target and onto the queue.
     */
    void QueueSession::enqueueFromUser()
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

        queue_.enqueue(Target(fieldOne, fieldTwo));
        Display::printMessage("Enqueued target at the back of the queue.");
    }

    /*
     * Purpose: Dequeue the front Target and show it.
     * Design: Guards the empty case so dequeue never throws to the menu loop.
     * Workflow: Report empty queues, otherwise dequeue and display the removed Target.
     * Data Handoff: Moves the removed Target from the queue to the display layer.
     */
    void QueueSession::dequeueAndShow()
    {
        if (queue_.isEmpty())
        {
            Display::printMessage("The queue is empty.");
            return;
        }

        const Target removed = queue_.dequeue();
        Display::printMessage("Dequeued: " + removed.toDisplayString());
    }

    /*
     * Purpose: Show the front Target without removing it.
     * Design: Guards the empty case so peeking never throws to the menu loop.
     * Workflow: Report empty queues, otherwise display the front Target.
     * Data Handoff: Reads the front Target and sends it to the display layer.
     */
    void QueueSession::peekAndShow() const
    {
        if (queue_.isEmpty())
        {
            Display::printMessage("The queue is empty.");
            return;
        }

        Display::printMessage("Front: " + queue_.peek().toDisplayString());
    }

    /*
     * Purpose: Provide mutable access to the backing queue.
     * Design: Exposes the queue for tests and advanced operations.
     * Workflow: Return queue_ by reference.
     * Data Handoff: Gives callers direct access to the session's TargetQueue.
     */
    TargetQueue& QueueSession::queue()
    {
        return queue_;
    }

    /*
     * Purpose: Provide read-only access to the backing queue.
     * Design: Supports const sessions without allowing mutation.
     * Workflow: Return queue_ by const reference.
     * Data Handoff: Gives callers read access to the session's TargetQueue.
     */
    const TargetQueue& QueueSession::queue() const
    {
        return queue_;
    }
}
