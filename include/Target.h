/*
 * File: Target.h
 * Description: Declares the generic Target value object used for simple two-field records.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Header.h"


// named container llb = linked list browser
namespace llb
{
    class Target
    {
    private:
        std::string fieldOne_;
        std::string fieldTwo_;

        static std::string toLowerCopy(std::string text);

    public:
        Target();
        Target(std::string fieldOne, std::string fieldTwo);

        const std::string& fieldOne() const;
        const std::string& fieldTwo() const;

        void setFieldOne(const std::string& fieldOne);
        void setFieldTwo(const std::string& fieldTwo);

        bool matches(const std::string& searchTerm) const;
        std::string toDisplayString() const;
    };
}
