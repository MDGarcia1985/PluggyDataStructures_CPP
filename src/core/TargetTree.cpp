/*
 * File: TargetTree.cpp
 * Description: Implements binary search tree insertion, search, removal, and traversals.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "core/TargetTree.h"
#include "sorting/SortSupport.h"

#include <queue>


// named container llb = linked list browser
namespace llb
{
    namespace
    {
        /*
         * Purpose: Normalize a key for case-insensitive descent comparisons.
         * Design: Works on a copy so stored Target values are never altered.
         * Workflow: Lowercase each byte safely and return the converted text.
         * Data Handoff: Supplies findByKey() with comparable text derived from a Target field.
         */
        std::string lowercaseKey(std::string text)
        {
            std::transform(text.begin(), text.end(), text.begin(),
                [](unsigned char character)
                {
                    return static_cast<char>(std::tolower(character));
                });
            return text;
        }

        /*
         * Purpose: Decide whether two Targets are equal under the shared ordering.
         * Design: Reuses targetLess so the tree's equality matches its ordering invariant.
         * Workflow: Two values are equal when neither is strictly less than the other.
         * Data Handoff: Supports duplicate rejection and node matching during search and removal.
         */
        bool targetsEqual(const Target& left, const Target& right)
        {
            return !targetLess(left, right) && !targetLess(right, left);
        }
    }

    /*
     * Purpose: Build one tree node around a Target value.
     * Design: Each node owns its Target and starts with no children.
     * Workflow: Copy the provided Target and null both child links.
     * Data Handoff: Receives a Target from insertInto() and joins it to the tree.
     */
    TargetTree::Node::Node(const Target& target)
        : data(target), left(nullptr), right(nullptr)
    {
    }

    /*
     * Purpose: Create an empty binary search tree.
     * Design: Tracks the root and a cached node count.
     * Workflow: Start with a null root and a zero count.
     * Data Handoff: Produces a ready-to-fill tree for a TreeSession or tests.
     */
    TargetTree::TargetTree()
        : root_(nullptr), count_(0)
    {
    }

    /*
     * Purpose: Create a deep copy of another tree.
     * Design: Recursively clones nodes so the two trees can change independently.
     * Workflow: Copy the source structure and copy the node count.
     * Data Handoff: Receives another tree and reproduces its shape and values here.
     */
    TargetTree::TargetTree(const TargetTree& other)
        : root_(copyTree(other.root_)), count_(other.count_)
    {
    }

    /*
     * Purpose: Move another tree into this object without cloning nodes.
     * Design: Transfers the root pointer and empties the source.
     * Workflow: Take the source root and count, then reset the source to empty.
     * Data Handoff: Receives ownership of another tree's nodes.
     */
    TargetTree::TargetTree(TargetTree&& other) noexcept
        : root_(other.root_), count_(other.count_)
    {
        other.root_ = nullptr;
        other.count_ = 0;
    }

    /*
     * Purpose: Replace this tree with a deep copy of another tree.
     * Design: Builds the copy first, then releases existing nodes, for exception safety.
     * Workflow: Guard self-assignment, clone the source, clear this tree, and adopt the clone.
     * Data Handoff: Receives values from other and returns this tree by reference.
     */
    TargetTree& TargetTree::operator=(const TargetTree& other)
    {
        if (this != &other)
        {
            Node* copiedRoot = copyTree(other.root_);
            clear();
            root_ = copiedRoot;
            count_ = other.count_;
        }

        return *this;
    }

    /*
     * Purpose: Replace this tree by taking ownership of another tree's nodes.
     * Design: Clears current nodes first, then transfers pointers from the source.
     * Workflow: Guard self-assignment, clear this tree, move the root and count, and empty the source.
     * Data Handoff: Receives node ownership from other and returns this tree by reference.
     */
    TargetTree& TargetTree::operator=(TargetTree&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            root_ = other.root_;
            count_ = other.count_;
            other.root_ = nullptr;
            other.count_ = 0;
        }

        return *this;
    }

    /*
     * Purpose: Release all dynamic memory owned by the tree.
     * Design: Delegates to clear() so destructor behavior matches manual clearing.
     * Workflow: Free every node through clear().
     * Data Handoff: Returns memory to the program when the tree goes out of scope.
     */
    TargetTree::~TargetTree()
    {
        clear();
    }

    /*
     * Purpose: Remove every node from the tree.
     * Design: Frees nodes iteratively so deep trees do not exhaust the call stack.
     * Workflow: Walk all nodes with an explicit stack, deleting each one, then reset state.
     * Data Handoff: Consumes the current node structure and leaves an empty tree behind.
     */
    void TargetTree::clear()
    {
        destroyTree(root_);
        root_ = nullptr;
        count_ = 0;
    }

    /*
     * Purpose: Report whether the tree has no nodes.
     * Design: Uses the cached count for constant-time access.
     * Workflow: Compare count_ to zero.
     * Data Handoff: Returns a boolean to callers before traversal or removal.
     */
    bool TargetTree::isEmpty() const
    {
        return count_ == 0;
    }

    /*
     * Purpose: Report how many Targets are stored.
     * Design: Returns the cached count for constant-time access.
     * Workflow: Read count_ directly.
     * Data Handoff: Gives callers the tree size without exposing node pointers.
     */
    std::size_t TargetTree::size() const
    {
        return count_;
    }

    /*
     * Purpose: Report the height of the tree.
     * Design: Defines an empty tree as height zero and a single node as height one.
     * Workflow: Recursively measure the deeper subtree from the root.
     * Data Handoff: Gives callers a balance indicator for the current shape.
     */
    std::size_t TargetTree::height() const
    {
        return heightOf(root_);
    }

    /*
     * Purpose: Insert a Target into the tree in sorted position.
     * Design: Uses the shared ordering and rejects exact duplicates.
     * Workflow: Descend recursively, create a node at the empty slot, and increment the count.
     * Data Handoff: Receives a Target and reports whether a new node was created.
     */
    bool TargetTree::insert(const Target& target)
    {
        bool inserted = false;
        root_ = insertInto(root_, target, inserted);
        if (inserted)
        {
            ++count_;
        }
        return inserted;
    }

    /*
     * Purpose: Report whether an exact Target exists in the tree.
     * Design: Descends with the shared ordering for an O(height) search.
     * Workflow: Compare at each node, returning on a match or descending left/right.
     * Data Handoff: Receives a Target and returns whether it is stored.
     */
    bool TargetTree::contains(const Target& target) const
    {
        const Node* node = root_;
        while (node != nullptr)
        {
            if (targetsEqual(target, node->data))
            {
                return true;
            }
            node = targetLess(target, node->data) ? node->left : node->right;
        }
        return false;
    }

    /*
     * Purpose: Find a stored Target by its first-field key.
     * Design: Descends using the primary sort key so lookups stay O(height) for typical data.
     * Workflow: Compare lowercased keys, returning a match or moving left/right by key order.
     * Data Handoff: Writes the matched Target into found and reports whether a match existed.
     */
    bool TargetTree::findByKey(const std::string& key, Target& found) const
    {
        const std::string normalizedKey = lowercaseKey(key);
        const Node* node = root_;

        while (node != nullptr)
        {
            const std::string nodeKey = lowercaseKey(node->data.fieldOne());
            if (normalizedKey == nodeKey)
            {
                found = node->data;
                return true;
            }
            node = normalizedKey < nodeKey ? node->left : node->right;
        }

        return false;
    }

    /*
     * Purpose: Remove an exact Target from the tree.
     * Design: Uses standard BST deletion that handles leaf, single-child, and two-child cases.
     * Workflow: Descend recursively, splice out the node, and decrement the count on success.
     * Data Handoff: Receives a Target and reports whether a node was removed.
     */
    bool TargetTree::remove(const Target& target)
    {
        bool removed = false;
        root_ = removeFrom(root_, target, removed);
        if (removed)
        {
            --count_;
        }
        return removed;
    }

    /*
     * Purpose: Remove a stored Target located by its first-field key.
     * Design: Reuses findByKey so removal targets the same node a key lookup would return.
     * Workflow: Look up the Target by key, then remove that exact value if found.
     * Data Handoff: Receives a key and reports whether a matching node was removed.
     */
    bool TargetTree::removeByKey(const std::string& key)
    {
        Target found;
        if (!findByKey(key, found))
        {
            return false;
        }
        return remove(found);
    }

    /*
     * Purpose: Return Targets in ascending (in-order) sequence.
     * Design: Produces a sorted snapshot using the tree's ordering invariant.
     * Workflow: Recursively visit left, node, then right, collecting values.
     * Data Handoff: Returns a vector snapshot for display or tests.
     */
    std::vector<Target> TargetTree::inOrder() const
    {
        std::vector<Target> output;
        output.reserve(count_);
        inOrder(root_, output);
        return output;
    }

    /*
     * Purpose: Return Targets in pre-order (node, left, right) sequence.
     * Design: Reveals the tree's root-first structure for teaching and verification.
     * Workflow: Recursively visit node, left, then right, collecting values.
     * Data Handoff: Returns a vector snapshot for display or tests.
     */
    std::vector<Target> TargetTree::preOrder() const
    {
        std::vector<Target> output;
        output.reserve(count_);
        preOrder(root_, output);
        return output;
    }

    /*
     * Purpose: Return Targets in post-order (left, right, node) sequence.
     * Design: Reveals the children-first structure used by safe deletion.
     * Workflow: Recursively visit left, right, then node, collecting values.
     * Data Handoff: Returns a vector snapshot for display or tests.
     */
    std::vector<Target> TargetTree::postOrder() const
    {
        std::vector<Target> output;
        output.reserve(count_);
        postOrder(root_, output);
        return output;
    }

    /*
     * Purpose: Return Targets in breadth-first (level-order) sequence.
     * Design: Uses a FIFO queue of nodes to visit each depth before the next.
     * Workflow: Enqueue the root, then repeatedly dequeue a node and enqueue its children.
     * Data Handoff: Returns a vector snapshot for display or tests.
     */
    std::vector<Target> TargetTree::levelOrder() const
    {
        std::vector<Target> output;
        output.reserve(count_);

        std::queue<const Node*> pending;
        if (root_ != nullptr)
        {
            pending.push(root_);
        }

        while (!pending.empty())
        {
            const Node* node = pending.front();
            pending.pop();
            output.push_back(node->data);

            if (node->left != nullptr)
            {
                pending.push(node->left);
            }
            if (node->right != nullptr)
            {
                pending.push(node->right);
            }
        }

        return output;
    }

    /*
     * Purpose: Recursively clone a subtree.
     * Design: Creates new nodes mirroring the source structure and values.
     * Workflow: Copy the current node, then clone its left and right subtrees.
     * Data Handoff: Returns an independent copy of the supplied subtree.
     */
    TargetTree::Node* TargetTree::copyTree(const Node* source)
    {
        if (source == nullptr)
        {
            return nullptr;
        }

        Node* copied = new Node(source->data);
        copied->left = copyTree(source->left);
        copied->right = copyTree(source->right);
        return copied;
    }

    /*
     * Purpose: Free an entire subtree without deep recursion.
     * Design: Uses an explicit stack so pathological trees cannot overflow the call stack.
     * Workflow: Push the root, then pop each node, scheduling its children before deleting it.
     * Data Handoff: Consumes the supplied subtree and returns its memory to the program.
     */
    void TargetTree::destroyTree(Node* node)
    {
        std::vector<Node*> pending;
        if (node != nullptr)
        {
            pending.push_back(node);
        }

        while (!pending.empty())
        {
            Node* current = pending.back();
            pending.pop_back();

            if (current->left != nullptr)
            {
                pending.push_back(current->left);
            }
            if (current->right != nullptr)
            {
                pending.push_back(current->right);
            }

            delete current;
        }
    }

    /*
     * Purpose: Measure the height of a subtree.
     * Design: Treats a null subtree as height zero for a simple recursive definition.
     * Workflow: Return one plus the height of the taller child subtree.
     * Data Handoff: Supplies height() with the recursive measurement.
     */
    std::size_t TargetTree::heightOf(const Node* node)
    {
        if (node == nullptr)
        {
            return 0;
        }

        const std::size_t leftHeight = heightOf(node->left);
        const std::size_t rightHeight = heightOf(node->right);
        return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    }

    /*
     * Purpose: Find the smallest node in a subtree.
     * Design: Follows left links because the leftmost node holds the minimum.
     * Workflow: Descend left until a node has no left child.
     * Data Handoff: Supplies removeFrom() with the in-order successor for two-child deletion.
     */
    TargetTree::Node* TargetTree::minimumNode(Node* node)
    {
        while (node != nullptr && node->left != nullptr)
        {
            node = node->left;
        }
        return node;
    }

    /*
     * Purpose: Append in-order values from a subtree.
     * Design: Standard left, node, right recursion.
     * Workflow: Recurse left, record the node, then recurse right.
     * Data Handoff: Fills the output vector for inOrder().
     */
    void TargetTree::inOrder(const Node* node, std::vector<Target>& output) const
    {
        if (node == nullptr)
        {
            return;
        }
        inOrder(node->left, output);
        output.push_back(node->data);
        inOrder(node->right, output);
    }

    /*
     * Purpose: Append pre-order values from a subtree.
     * Design: Standard node, left, right recursion.
     * Workflow: Record the node, then recurse left and right.
     * Data Handoff: Fills the output vector for preOrder().
     */
    void TargetTree::preOrder(const Node* node, std::vector<Target>& output) const
    {
        if (node == nullptr)
        {
            return;
        }
        output.push_back(node->data);
        preOrder(node->left, output);
        preOrder(node->right, output);
    }

    /*
     * Purpose: Append post-order values from a subtree.
     * Design: Standard left, right, node recursion.
     * Workflow: Recurse left and right, then record the node.
     * Data Handoff: Fills the output vector for postOrder().
     */
    void TargetTree::postOrder(const Node* node, std::vector<Target>& output) const
    {
        if (node == nullptr)
        {
            return;
        }
        postOrder(node->left, output);
        postOrder(node->right, output);
        output.push_back(node->data);
    }

    /*
     * Purpose: Insert a Target into a subtree, rejecting exact duplicates.
     * Design: Returns the (possibly new) subtree root so links stay consistent.
     * Workflow: Create a node at an empty slot, otherwise descend by the shared ordering.
     * Data Handoff: Reports creation through the inserted flag and returns the updated subtree.
     */
    TargetTree::Node* TargetTree::insertInto(Node* node, const Target& target, bool& inserted)
    {
        if (node == nullptr)
        {
            inserted = true;
            return new Node(target);
        }

        if (targetsEqual(target, node->data))
        {
            inserted = false;
            return node;
        }

        if (targetLess(target, node->data))
        {
            node->left = insertInto(node->left, target, inserted);
        }
        else
        {
            node->right = insertInto(node->right, target, inserted);
        }

        return node;
    }

    /*
     * Purpose: Remove a Target from a subtree.
     * Design: Handles leaf, single-child, and two-child cases with the in-order successor.
     * Workflow: Descend to the match, splice it out, and report success through removed.
     * Data Handoff: Returns the updated subtree root after deletion.
     */
    TargetTree::Node* TargetTree::removeFrom(Node* node, const Target& target, bool& removed)
    {
        if (node == nullptr)
        {
            removed = false;
            return nullptr;
        }

        if (targetLess(target, node->data))
        {
            node->left = removeFrom(node->left, target, removed);
            return node;
        }

        if (targetLess(node->data, target))
        {
            node->right = removeFrom(node->right, target, removed);
            return node;
        }

        removed = true;

        if (node->left == nullptr)
        {
            Node* child = node->right;
            delete node;
            return child;
        }

        if (node->right == nullptr)
        {
            Node* child = node->left;
            delete node;
            return child;
        }

        Node* successor = minimumNode(node->right);
        node->data = successor->data;
        bool successorRemoved = false;
        node->right = removeFrom(node->right, successor->data, successorRemoved);
        return node;
    }
}
