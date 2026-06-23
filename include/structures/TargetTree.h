/*
 * File: TargetTree.h
 * Description: Declares a binary search tree of generic Target records with traversals.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/Target.h"

#include <functional>


// named container pds = Pluggy Data Structures
namespace pds
{
    enum class TreeVisitOrder
    {
        InOrder,
        PreOrder,
        PostOrder,
        LevelOrder
    };

    class TargetTree
    {
    private:
        struct Node
        {
            Target data;
            Node* left;
            Node* right;

            explicit Node(const Target& target);
        };

        Node* root_;
        std::size_t count_;

        static Node* copyTree(const Node* source);
        static void destroyTree(Node* node);
        static Node* minimumNode(Node* node);
        static std::size_t heightOf(const Node* node);
        static std::size_t leafCountOf(const Node* node);
        static bool checkBalanced(const Node* node, std::size_t& outHeight);
        static void visitInOrder(const Node* node, const std::function<void(const Target&)>& visitor);
        static void visitPreOrder(const Node* node, const std::function<void(const Target&)>& visitor);
        static void visitPostOrder(const Node* node, const std::function<void(const Target&)>& visitor);

        Node* insertInto(Node* node, const Target& target, bool& inserted);
        Node* removeFrom(Node* node, const Target& target, bool& removed);

    public:
        TargetTree();
        TargetTree(const TargetTree& other);
        TargetTree(TargetTree&& other) noexcept;
        TargetTree& operator=(const TargetTree& other);
        TargetTree& operator=(TargetTree&& other) noexcept;
        ~TargetTree();

        void clear();
        bool isEmpty() const;
        std::size_t size() const;

        bool insert(const Target& target);
        bool contains(const Target& target) const;
        bool findByKey(const std::string& key, Target& found) const;
        bool remove(const Target& target);
        bool removeByKey(const std::string& key);

        void visit(TreeVisitOrder order, const std::function<void(const Target&)>& visitor) const;
        std::size_t height() const;
        std::size_t leafCount() const;
        bool isBalanced() const;
        bool minimum(Target& found) const;
        bool maximum(Target& found) const;
        std::vector<Target> pathToKey(const std::string& key) const;
    };
}
