/*
 * File: TestMain.cpp
 * Description: Runs every registered test case and returns a CI-friendly exit code.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "TestHarness.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <set>


namespace
{
    struct TestResult
    {
        std::string name;
        bool passed;
        std::string message;
    };

    std::string currentTimestamp()
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm localTime{};

#if defined(_WIN32)
        localtime_s(&localTime, &nowTime);
#else
        localtime_r(&nowTime, &localTime);
#endif

        std::ostringstream formatted;
        formatted << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return formatted.str();
    }

    std::set<std::string> readLastLoggedTests(const std::filesystem::path& logPath)
    {
        std::ifstream input(logPath);
        std::set<std::string> latestTests;
        std::set<std::string> currentBlock;
        bool readingBlock = false;

        for (std::string line; std::getline(input, line);)
        {
            if (line == "BEGIN_CURRENT_TESTS")
            {
                currentBlock.clear();
                readingBlock = true;
                continue;
            }

            if (line == "END_CURRENT_TESTS")
            {
                latestTests = currentBlock;
                readingBlock = false;
                continue;
            }

            if (readingBlock && !line.empty())
            {
                currentBlock.insert(line);
            }
        }

        return latestTests;
    }

    void appendTestLog(const std::vector<TestResult>& results, int passed, int failed)
    {
#if defined(PDS_TEST_LOG_DIR)
        const std::filesystem::path logDirectory = PDS_TEST_LOG_DIR;
#else
        const std::filesystem::path logDirectory = std::filesystem::path("tests") / "data";
#endif
        const std::filesystem::path logPath = logDirectory / "test-log.txt";

        std::error_code directoryError;
        std::filesystem::create_directories(logDirectory, directoryError);

        const std::set<std::string> previousTests = readLastLoggedTests(logPath);
        std::vector<std::string> currentTests;
        std::vector<std::string> newTests;
        currentTests.reserve(results.size());

        for (const TestResult& result : results)
        {
            currentTests.push_back(result.name);
            if (previousTests.find(result.name) == previousTests.end())
            {
                newTests.push_back(result.name);
            }
        }

        std::ofstream log(logPath, std::ios::app);
        if (!log)
        {
            std::cerr << "WARNING: Could not open test log at " << logPath.string() << '\n';
            return;
        }

        log << "============================================================\n";
        log << "Test run: " << currentTimestamp() << '\n';
        log << "Result: " << (failed == 0 ? "PASS" : "FAIL") << '\n';
        log << "Tests run: " << results.size() << '\n';
        log << "Passed: " << passed << '\n';
        log << "Failed: " << failed << '\n';
        log << "Assertions checked: " << pdstest::assertionCount() << "\n\n";

        log << "New tests since previous run:\n";
        if (previousTests.empty())
        {
            log << "  No previous run found; treating all tests as current baseline.\n";
        }
        else if (newTests.empty())
        {
            log << "  none\n";
        }
        else
        {
            for (const std::string& testName : newTests)
            {
                log << "  " << testName << '\n';
            }
        }

        log << "\nMajor test results:\n";
        for (const TestResult& result : results)
        {
            log << "  " << (result.passed ? "PASS" : "FAIL") << " " << result.name;
            if (!result.passed && !result.message.empty())
            {
                log << ": " << result.message;
            }
            log << '\n';
        }

        log << "\nBEGIN_CURRENT_TESTS\n";
        for (const std::string& testName : currentTests)
        {
            log << testName << '\n';
        }
        log << "END_CURRENT_TESTS\n\n";
    }
}


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
    std::vector<TestResult> results;

    for (const pdstest::TestCase& testCase : pdstest::TestRegistry::instance().cases())
    {
        try
        {
            testCase.run();
            ++passed;
            results.push_back(TestResult{testCase.name, true, ""});
        }
        catch (const std::exception& error)
        {
            ++failed;
            results.push_back(TestResult{testCase.name, false, error.what()});
            std::cerr << "FAILED: " << testCase.name << ": " << error.what() << '\n';
        }
    }

    std::cout << '\n'
        << passed << " test case(s) passed, "
        << failed << " failed, "
        << pdstest::assertionCount() << " assertion(s) checked.\n";

    appendTestLog(results, passed, failed);

    return failed == 0 ? 0 : 1;
}
