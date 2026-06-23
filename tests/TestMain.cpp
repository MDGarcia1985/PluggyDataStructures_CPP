/*
 * File: TestMain.cpp
 * Description: Runs every registered test case and returns a CI-friendly exit code.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "TestHarness.h"


/*
 * Purpose: Execute all registered test cases and summarize the results.
 * Design: Isolates each case in a try block so one failure does not stop the suite.
 * Workflow: Run each case, count passes and failures, print a summary, and return nonzero on failure.
 * Data Handoff: Reads cases from the shared TestRegistry and writes results to the console.
 */
int main()
{
    int passed = 0;
    int failed = 0;

    for (const pdstest::TestCase& testCase : pdstest::TestRegistry::instance().cases())
    {
        try
        {
            testCase.run();
            ++passed;
        }
        catch (const std::exception& error)
        {
            ++failed;
            std::cerr << "FAILED: " << testCase.name << ": " << error.what() << '\n';
        }
    }

    std::cout << '\n'
        << passed << " test case(s) passed, "
        << failed << " failed, "
        << pdstest::assertionCount() << " assertion(s) checked.\n";

    return failed == 0 ? 0 : 1;
}
