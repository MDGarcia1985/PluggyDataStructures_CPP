/*
 * File: Website.h
 * Description: Declares the Website value object used for names, URLs, and matching.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Website: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Header.h"


// named container llb = linked list browser
namespace llb
{
    class Website
    {
    private:
        std::string name_;
        std::string url_;

        static std::string toLowerCopy(std::string text);

    public:
        Website();
        Website(std::string name, std::string url);

        const std::string& name() const;
        const std::string& url() const;

        void setName(const std::string& name);
        void setUrl(const std::string& url);

        bool matches(const std::string& searchTerm) const;
        std::string toDisplayString() const;
    };
}
