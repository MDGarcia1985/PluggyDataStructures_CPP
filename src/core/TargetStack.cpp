#include "core/TargetStack.h"
#include "ui/Display.h"


// named container llb = linked list browser
namespace llb
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
     * Design: Treats the list tail as the stack top to demonstrate LIFO behavior.
     * Workflow: Snapshot the list, copy the last target, remove the last node, and return the copy.
     * Data Handoff: Returns the removed Target to the caller or throws if the stack is empty.
     */
    Target TargetStack::pop()
    {
        if (isEmpty())
        {
            throw std::runtime_error("Cannot pop from an empty target stack.");
        }

        const std::vector<Target> targets = stack_.toVector();
        Target top = targets.back();
        stack_.removeAt(stack_.size());
        return top;
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
