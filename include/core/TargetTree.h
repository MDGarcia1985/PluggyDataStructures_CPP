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


// named container llb = linked list browser
namespace llb
{
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
        static std::size_t heightOf(const Node* node);
        static Node* minimumNode(Node* node);

        void inOrder(const Node* node, std::vector<Target>& output) const;
        void preOrder(const Node* node, std::vector<Target>& output) const;
        void postOrder(const Node* node, std::vector<Target>& output) const;

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
        std::size_t height() const;

        bool insert(const Target& target);
        bool contains(const Target& target) const;
        bool findByKey(const std::string& key, Target& found) const;
        bool remove(const Target& target);
        bool removeByKey(const std::string& key);

        std::vector<Target> inOrder() const;
        std::vector<Target> preOrder() const;
        std::vector<Target> postOrder() const;
        std::vector<Target> levelOrder() const;
    };
}
