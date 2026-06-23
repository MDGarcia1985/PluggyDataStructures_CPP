/*
 * File: TargetTree.cpp
 * Description: Implements binary search tree insertion, search, removal, and traversals.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "structures/TargetTree.h"

#include "algorithms/common/KeyNormalization.h"
#include "algorithms/common/TargetOrdering.h"

#include <queue>


// named container pds = Pluggy Data Structures
namespace pds
{
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
            if (targetEqualsByOrdering(target, node->data))
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
        const std::string normalizedKey = normalizeKey(key);
        const Node* node = root_;

        while (node != nullptr)
        {
            const std::string nodeKey = normalizeKey(node->data.fieldOne());
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

    void TargetTree::visit(TreeVisitOrder order, const std::function<void(const Target&)>& visitor) const
    {
        if (!visitor)
        {
            return;
        }

        if (order == TreeVisitOrder::InOrder)
        {
            visitInOrder(root_, visitor);
            return;
        }
        if (order == TreeVisitOrder::PreOrder)
        {
            visitPreOrder(root_, visitor);
            return;
        }
        if (order == TreeVisitOrder::PostOrder)
        {
            visitPostOrder(root_, visitor);
            return;
        }

        std::queue<const Node*> pending;
        if (root_ != nullptr)
        {
            pending.push(root_);
        }

        while (!pending.empty())
        {
            const Node* node = pending.front();
            pending.pop();
            visitor(node->data);

            if (node->left != nullptr)
            {
                pending.push(node->left);
            }
            if (node->right != nullptr)
            {
                pending.push(node->right);
            }
        }
    }

    std::size_t TargetTree::height() const
    {
        return heightOf(root_);
    }

    std::size_t TargetTree::leafCount() const
    {
        return leafCountOf(root_);
    }

    bool TargetTree::isBalanced() const
    {
        std::size_t measuredHeight = 0;
        return checkBalanced(root_, measuredHeight);
    }

    bool TargetTree::minimum(Target& found) const
    {
        const Node* node = root_;
        if (node == nullptr)
        {
            return false;
        }
        while (node->left != nullptr)
        {
            node = node->left;
        }
        found = node->data;
        return true;
    }

    bool TargetTree::maximum(Target& found) const
    {
        const Node* node = root_;
        if (node == nullptr)
        {
            return false;
        }
        while (node->right != nullptr)
        {
            node = node->right;
        }
        found = node->data;
        return true;
    }

    std::vector<Target> TargetTree::pathToKey(const std::string& key) const
    {
        const std::string normalizedKey = normalizeKey(key);
        std::vector<Target> path;
        const Node* node = root_;

        while (node != nullptr)
        {
            path.push_back(node->data);
            const std::string nodeKey = normalizeKey(node->data.fieldOne());
            if (normalizedKey == nodeKey)
            {
                return path;
            }
            node = normalizedKey < nodeKey ? node->left : node->right;
        }

        return std::vector<Target>();
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

    std::size_t TargetTree::leafCountOf(const Node* node)
    {
        if (node == nullptr)
        {
            return 0;
        }
        if (node->left == nullptr && node->right == nullptr)
        {
            return 1;
        }
        return leafCountOf(node->left) + leafCountOf(node->right);
    }

    bool TargetTree::checkBalanced(const Node* node, std::size_t& outHeight)
    {
        if (node == nullptr)
        {
            outHeight = 0;
            return true;
        }

        std::size_t leftHeight = 0;
        std::size_t rightHeight = 0;
        const bool leftBalanced = checkBalanced(node->left, leftHeight);
        const bool rightBalanced = checkBalanced(node->right, rightHeight);

        outHeight = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
        const std::size_t difference = leftHeight > rightHeight
            ? leftHeight - rightHeight
            : rightHeight - leftHeight;
        return leftBalanced && rightBalanced && difference <= 1;
    }

    void TargetTree::visitInOrder(const Node* node, const std::function<void(const Target&)>& visitor)
    {
        if (node == nullptr)
        {
            return;
        }
        visitInOrder(node->left, visitor);
        visitor(node->data);
        visitInOrder(node->right, visitor);
    }

    void TargetTree::visitPreOrder(const Node* node, const std::function<void(const Target&)>& visitor)
    {
        if (node == nullptr)
        {
            return;
        }
        visitor(node->data);
        visitPreOrder(node->left, visitor);
        visitPreOrder(node->right, visitor);
    }

    void TargetTree::visitPostOrder(const Node* node, const std::function<void(const Target&)>& visitor)
    {
        if (node == nullptr)
        {
            return;
        }
        visitPostOrder(node->left, visitor);
        visitPostOrder(node->right, visitor);
        visitor(node->data);
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

        if (targetEqualsByOrdering(target, node->data))
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
