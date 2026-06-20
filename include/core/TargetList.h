/*
 * File: TargetList.h
 * Description: Declares the doubly linked list that stores and navigates generic targets.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"
#include "core/Target.h"


// named container pds = Pluggy Data Structures
namespace pds
{
    struct TargetSearchResult
    {
        std::size_t position;
        Target target;
    };

    class TargetList
    {
    private:
        struct Node
        {
            Target data;
            Node* next;
            Node* previous;

            explicit Node(const Target& target);
        };

        Node* head_;
        Node* tail_;
        Node* current_;
        std::size_t count_;

        Node* nodeAt(std::size_t position) const;
        void copyFrom(const TargetList& other);
        void swap(TargetList& other) noexcept;

    public:
        TargetList();
        TargetList(const TargetList& other);
        TargetList(TargetList&& other) noexcept;
        TargetList& operator=(const TargetList& other);
        TargetList& operator=(TargetList&& other) noexcept;
        ~TargetList();

        void clear();
        bool isEmpty() const;
        std::size_t size() const;

        void addBack(const Target& target);
        bool removeAt(std::size_t position);

        const Target* front() const;
        const Target* back() const;
        bool removeFront();
        bool removeBack();

        bool moveForward();
        bool moveBackward();
        bool setCurrentToPosition(std::size_t position);

        const Target* current() const;
        std::size_t currentPosition() const;

        std::vector<Target> toVector() const;
        std::vector<TargetSearchResult> findAll(const std::string& searchTerm) const;
    };
}
