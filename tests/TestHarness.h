/*
 * File: TestHarness.h
 * Description: Minimal self-registering test harness shared by all test modules.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdio>
#include <functional>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


// Lightweight test utilities, modeled on the project's plugin-registration spine.
namespace llbtest
{
    /*
     * Purpose: Track how many assertions have run across all test modules.
     * Design: Uses a C++17 inline accessor so every translation unit shares one counter.
     * Workflow: Return a reference to the single static counter.
     * Data Handoff: Lets expect() report a global assertion total to the runner.
     */
    inline int& assertionCount()
    {
        static int count = 0;
        return count;
    }

    /*
     * Purpose: Assert a condition and throw with a message on failure.
     * Design: Throws so the runner can mark a test case failed without aborting the process.
     * Workflow: Count the assertion, then throw a runtime_error when the condition is false.
     * Data Handoff: Receives a condition and message from a test body.
     */
    inline void expect(bool condition, const std::string& message)
    {
        ++assertionCount();
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    /*
     * Purpose: Assert two strings are equal with a descriptive failure message.
     * Design: Wraps expect() and formats the expected and actual values.
     * Workflow: Compare the strings and report both values when they differ.
     * Data Handoff: Receives actual and expected text from a test body.
     */
    inline void expectEqual(const std::string& actual, const std::string& expected, const std::string& message)
    {
        expect(actual == expected, message + " Expected: [" + expected + "], actual: [" + actual + "]");
    }

    /*
     * Purpose: Assert two sizes are equal with a descriptive failure message.
     * Design: Wraps expect() and formats the expected and actual values.
     * Workflow: Compare the sizes and report both values when they differ.
     * Data Handoff: Receives actual and expected counts from a test body.
     */
    inline void expectEqual(std::size_t actual, std::size_t expected, const std::string& message)
    {
        expect(actual == expected,
            message + " Expected: " + std::to_string(expected) + ", actual: " + std::to_string(actual));
    }

    /*
     * Purpose: Report whether text contains a substring.
     * Design: Thin wrapper over std::string::find for readable assertions.
     * Workflow: Return whether the search term appears in the text.
     * Data Handoff: Supports console-output assertions in test bodies.
     */
    inline bool contains(const std::string& text, const std::string& searchTerm)
    {
        return text.find(searchTerm) != std::string::npos;
    }

    /*
     * Purpose: Write text content to a temporary file for loader tests.
     * Design: Truncates and writes in one call so fixtures are easy to set up.
     * Workflow: Open the path and stream the contents.
     * Data Handoff: Creates on-disk fixtures consumed by file-loading tests.
     */
    inline void writeFile(const std::string& path, const std::string& contents)
    {
        std::ofstream output(path);
        output << contents;
    }

    /*
     * Purpose: Capture std::cout output for assertions, restoring it on scope exit.
     * Design: Swaps the cout stream buffer in the constructor and back in the destructor.
     * Workflow: Redirect cout to an internal buffer, then restore the original buffer.
     * Data Handoff: Exposes captured console text to test bodies.
     */
    class ScopedCoutCapture
    {
    private:
        std::ostringstream captured_;
        std::streambuf* original_;

    public:
        ScopedCoutCapture()
            : captured_(), original_(std::cout.rdbuf(captured_.rdbuf()))
        {
        }

        ~ScopedCoutCapture()
        {
            std::cout.rdbuf(original_);
        }

        std::string text() const
        {
            return captured_.str();
        }
    };

    /*
     * Purpose: Feed scripted input to std::cin for interactive-flow tests.
     * Design: Swaps the cin stream buffer in the constructor and back in the destructor.
     * Workflow: Redirect cin to an internal buffer, then restore the original buffer.
     * Data Handoff: Supplies simulated user input to test bodies.
     */
    class ScopedCinInput
    {
    private:
        std::istringstream input_;
        std::streambuf* original_;

    public:
        explicit ScopedCinInput(const std::string& input)
            : input_(input), original_(std::cin.rdbuf(input_.rdbuf()))
        {
        }

        ~ScopedCinInput()
        {
            std::cin.rdbuf(original_);
        }
    };

    struct TestCase
    {
        std::string name;
        std::function<void()> run;
    };

    /*
     * Purpose: Collect every registered test case for the runner.
     * Design: Function-local static singleton mirrors the application's registry pattern.
     * Workflow: Test files add cases at static-initialization time; the runner reads them later.
     * Data Handoff: Stores named test functions and exposes them to TestMain.
     */
    class TestRegistry
    {
    public:
        static TestRegistry& instance()
        {
            static TestRegistry registry;
            return registry;
        }

        bool add(const std::string& name, std::function<void()> run)
        {
            cases_.push_back(TestCase{name, std::move(run)});
            return true;
        }

        const std::vector<TestCase>& cases() const
        {
            return cases_;
        }

    private:
        std::vector<TestCase> cases_;
    };
}

#define LLBTEST_CONCAT_IMPL(first, second) first##second
#define LLBTEST_CONCAT(first, second) LLBTEST_CONCAT_IMPL(first, second)

#define LLB_TEST(name)                                                                 \
    static void name();                                                                \
    static const bool LLBTEST_CONCAT(llb_test_registered_, __LINE__) =                 \
        ::llbtest::TestRegistry::instance().add(#name, name);                          \
    static void name()
