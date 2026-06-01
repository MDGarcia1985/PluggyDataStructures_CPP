#include "TargetQueue.h"
#include "Display.h"


// named container llb = linked list browser
namespace llb
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
     * Design: Treats the list head as the queue front to demonstrate FIFO behavior.
     * Workflow: Snapshot the list, copy the first target, remove the first node, and return the copy.
     * Data Handoff: Returns the removed Target to the caller or throws if the queue is empty.
     */
    Target TargetQueue::dequeue()
    {
        if (isEmpty())
        {
            throw std::runtime_error("Cannot dequeue from an empty target queue.");
        }

        const std::vector<Target> targets = queue_.toVector();
        Target front = targets.front();
        queue_.removeAt(1);
        return front;
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
