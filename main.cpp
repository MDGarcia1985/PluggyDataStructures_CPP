/*
 * File: main.cpp
 * Description: Defines the executable entry point for LinkedListBrowser.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "app/App.h"

/*
 * Purpose: Start the application and return its process status.
 * Design: Keeps the global entry point minimal by delegating startup to pds::runApp.
 * Workflow: Invoke the application runner and return its result to the operating system.
 * Data Handoff: Passes the application exit code from pds::runApp to the process host.
 */
int main()
{
    return pds::runApp();
}
