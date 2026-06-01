/*
 * File: WebsiteList.h
 * Description: Declares the doubly linked list that stores and navigates websites.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Website: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Header.h"
#include "Website.h"


// named container llb = linked list browser
namespace llb
{
    struct WebsiteSearchResult
    {
        std::size_t position;
        Website website;
    };

    class WebsiteList
    {
    private:
        struct Node
        {
            Website data;
            Node* next;
            Node* previous;

            explicit Node(const Website& website);
        };

        Node* head_;
        Node* tail_;
        Node* current_;
        std::size_t count_;

        Node* nodeAt(std::size_t position) const;
        void copyFrom(const WebsiteList& other);
        void swap(WebsiteList& other) noexcept;

    public:
        WebsiteList();
        WebsiteList(const WebsiteList& other);
        WebsiteList(WebsiteList&& other) noexcept;
        WebsiteList& operator=(const WebsiteList& other);
        WebsiteList& operator=(WebsiteList&& other) noexcept;
        ~WebsiteList();

        void clear();
        bool isEmpty() const;
        std::size_t size() const;

        void addBack(const Website& website);
        bool removeAt(std::size_t position);

        bool moveForward();
        bool moveBackward();
        bool setCurrentToPosition(std::size_t position);

        const Website* current() const;
        std::size_t currentPosition() const;

        std::vector<Website> toVector() const;
        std::vector<WebsiteSearchResult> findAll(const std::string& searchTerm) const;
    };
}
