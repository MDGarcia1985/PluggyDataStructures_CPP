/*
 * File: DataSourceMenu.h
 * Description: Declares dataset discovery and selection flow.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/Header.h"


// named container llb = linked list browser
namespace llb
{
    class DataSourceMenu
    {
    public:
        static std::string selectDataSource();
    };
}
