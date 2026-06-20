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


// named container pds = Pluggy Data Structures
namespace pds
{
    class DataSourceMenu
    {
    public:
        static std::string selectDataSource();
    };
}
