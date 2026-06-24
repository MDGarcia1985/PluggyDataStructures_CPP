# Tests

This directory contains the project's C++ test suite. The tests use a small
self-registering harness instead of an external test framework.

## Layout

- `TestHarness.h` defines the assertion helpers, console/input capture helpers,
  and the `PDS_TEST(name)` macro.
- `TestMain.cpp` runs every registered test case, prints the console summary,
  and appends the test history log.
- `*Tests.cpp` files group tests by project area, such as algorithms, target
  structures, maps, graphs, trees, and application flows.
- `data/` stores test-run artifacts that should live with the test suite.

## How tests work

Each test case is declared with `PDS_TEST(testName)`. The macro registers the
test function during program startup. `TestMain.cpp` then asks the shared
registry for all test cases, runs them one at a time, catches assertion failures,
and returns a nonzero exit code if any test fails.

Assertions use helpers from `pdstest`, including `expect()` and `expectEqual()`.
The harness counts assertions globally, but the log records only the major test
case result so the history stays readable.

## Running tests

Build the CMake test target, then run the generated `PluggyDataStructureTests`
executable or use CTest from the build directory:

```powershell
ctest --output-on-failure
```

## Test log

Every test executable run appends a text entry to:

```text
tests/data/test-log.txt
```

Each entry includes the local run date and time, pass/fail summary, assertion
count, per-test pass/fail results, and tests that were added since the previous
logged run. On the first logged run, the current test list becomes the baseline.
