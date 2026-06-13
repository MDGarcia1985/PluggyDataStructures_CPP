# DEVNOTES

## Purpose

This file records the design decisions made while extending `LinkedListBrowser` from a generic linked-list browser into a plugin-driven data-structure test bed. The notes stop at the current electronics component sorting milestone, where `ElectronicsSortSupport` was separated from the `InsertionSort` and `SelectionSort` modules.

The next planned step is to make the same shape generic so any queued dataset can be sorted by any registered sorting algorithm.

## Current Architectural Direction

`LinkedListBrowser` began as a website-specific linked-list assignment. The project has since moved toward a generic `Target` architecture.

The central decision is that the linked list stores generic `Target` records. A `Target` has two fields:

```text
fieldOne
fieldTwo
```

The list should not know whether those fields represent a website, message, task, inventory item, electronics component, or another dataset row. Meaning comes from the loader, plugin, and menu text around the data.

Current baseline flow:

```text
data/<selected_file>
-> loader or plugin adapter
-> Target
-> TargetList
-> TargetProgram
-> Menu
-> Display
```

## Core Boundary Decisions

### `TargetList` remains domain-neutral

`TargetList` owns linked-list behavior only. It should not know about websites, electronics components, CSV files, menu prompts, sorting algorithms, or display formatting.

Accepted responsibilities:

```text
add Target
remove Target
clear list
navigate current item
search Targets
copy to std::vector<Target>
manage node memory
```

Rejected responsibilities:

```text
parse files
format console output
choose datasets
know electronics columns
own sorting policy
own plugin registration
```

### `Target` remains a simple two-field record

No electronics-specific fields were added to `Target`. Electronics rows are adapted into the existing shape:

```text
fieldOne = primary display / sort key, such as ref_des
fieldTwo = combined detail string
```

This keeps the existing linked-list code reusable.

### `Header.h` stays small

`Header.h` is for shared standard library includes, constants, and tiny common utilities only. It should not become the place where new data domains, sort contracts, or plugin behaviors are dumped.

### `App.h` remains the top-level bundle

`App.h` gathers public project modules for `main.cpp`. It is not the correct place to include dataset-specific plugins or sort-specific support.

## Plugin Spine Decision

The existing menu system is the plugin spine.

`Menu.h` defines:

```cpp
struct CommandPlugin
{
    int id;
    std::string label;
    std::function<void(TargetProgram&)> action;
};
```

and provides the `LLB_REGISTER_COMMAND` macro so any compiled `.cpp` file can register a menu command.

Decision:

```text
New behavior should be added by compiling new plugin .cpp files,
not by editing TargetProgram's built-in command list every time.
```

This makes drag-and-drop plugin tests possible as long as the files are included by the existing build command:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude main.cpp src/*.cpp -o LinkedListBrowser
```

## Electronics Dataset Milestone

The electronics sample introduced a richer dataset than the original pipe-delimited `websites.txt` and `messages.txt` files.

Important decisions:

1. Keep electronics parsing out of `TargetList`.
2. Keep electronics parsing out of the core `FileLoader` for now.
3. Treat electronics support as a plugin-side adapter.
4. Convert electronics rows into generic `Target` objects.
5. Sort an array/vector snapshot rather than sorting linked-list nodes directly.
6. Time only the sorting algorithm, not file I/O, parsing, loading, or setup.
7. For small datasets, repeat the sort several times and report an average.

## Current Electronics Plugin Package

The current electronics sorting milestone uses this package shape:

```text
include/
  ElectronicsSortSupport.h

src/
  ElectronicsSortSupport.cpp
  InsertionSort.cpp
  SelectionSort.cpp

data/
  circuit_sample_two.csv
```

This is a four-file plugin/support package plus the dataset.

The header is part of the plugin package because multiple `.cpp` files need a shared compile-time contract.

## `ElectronicsSortSupport.h` Decision

`ElectronicsSortSupport.h` exists as the contract between the electronics support implementation and the individual sorting modules.

Current contract:

```cpp
using ElectronicsSortFunction = void (*)(std::vector<Target>&);

bool electronicsTargetLess(const Target& left, const Target& right);

void runElectronicsSortCommand(
    TargetProgram& program,
    const std::string& algorithmName,
    ElectronicsSortFunction sortFunction);
```

Decision:

```text
InsertionSort.cpp and SelectionSort.cpp should not duplicate CSV parsing,
row mapping, timing, or linked-list replacement.
```

The support header exposes only what the algorithm plugins need:

```text
sort function type
shared electronics comparator
shared electronics timing runner
```

## `ElectronicsSortSupport.cpp` Decision

`ElectronicsSortSupport.cpp` owns the electronics-specific workflow.

Accepted responsibilities:

```text
trim CSV fields
parse one CSV line
handle quoted CSV fields
map one electronics row into Target
load circuit_sample_two.csv
build natural component sort keys
compare electronics Targets
choose timing repetitions
clone arrays before timing
run the selected sort function
replace the active TargetList with the sorted result
report timing results
```

This keeps the algorithm modules small and avoids two copies of the same CSV loader.

### CSV parsing decision

The CSV parser is intentionally a small scanner instead of a naive comma split.

Reason:

```text
Commas can appear inside quoted CSV fields, so comma handling depends on quote state.
```

Design:

```text
outside quotes: comma ends a field
inside quotes: comma is data
doubled quote inside quotes: literal quote character
```

### Electronics row mapping decision

Electronics rows are normalized into `Target` objects.

Current mapping:

```text
Target::fieldOne = ref_des
Target::fieldTwo = readable attribute string
```

The detail string may include values such as:

```text
pin=<pin>; net=<net>; type=<type>; value=<value>; kind=<kind>; circuit=<circuit>; description=<description>
```

This lets the generic display and search code continue to work without electronics-specific changes.

### Natural component ordering decision

The support module provides `electronicsTargetLess()` so all electronics sorting algorithms use the same ordering rule.

Current comparison policy:

```text
compare reference-designator prefix
compare numeric portion numerically when both sides have a number
fall back to raw fieldOne
fall back to fieldTwo
```

Example design intent:

```text
R2 should sort before R10
```

The ordering policy is shared so insertion sort and selection sort do not accidentally define different versions of “sorted.”

### Timing decision

The timed region should include only calls to the sorting algorithm.

Accepted timing shape:

```text
load and clean data before timing
build baseline array before timing
clone arrays before timing
start clock
sort cloned arrays
stop clock
report total seconds
report average seconds per sort
replace active TargetList after timing
```

This avoids timing file I/O, CSV parsing, vector construction, linked-list construction, or console output.

## `InsertionSort.cpp` Separation Decision

`InsertionSort.cpp` is now a sorting strategy module.

Responsibilities:

```text
define insertion sort
use the shared electronics comparator
register an insertion-sort menu command
call the shared electronics timing runner
```

It should not:

```text
open the CSV file
parse rows
know column indexes
choose timing repetitions
replace the linked list directly
print the full timing report
```

Current behavior:

```text
insertionSort(std::vector<Target>& targets)
    sorts the supplied array in place
    compares with electronicsTargetLess()

electronicsInsertionSortCommand(TargetProgram& program)
    calls runElectronicsSortCommand(program, "insertion sort", insertionSort)
```

Menu registration:

```text
8) Load electronics CSV and time insertion sort
```

## `SelectionSort.cpp` Separation Decision

`SelectionSort.cpp` mirrors the insertion-sort module but owns selection sort.

Responsibilities:

```text
define selection sort
use the shared electronics comparator
register a selection-sort menu command
call the shared electronics timing runner
```

It should not:

```text
open the CSV file
parse rows
know column indexes
choose timing repetitions
replace the linked list directly
print the full timing report
```

Current behavior:

```text
selectionSort(std::vector<Target>& targets)
    sorts the supplied array in place
    compares with electronicsTargetLess()

electronicsSelectionSortCommand(TargetProgram& program)
    calls runElectronicsSortCommand(program, "selection sort", selectionSort)
```

Menu registration:

```text
9) Load electronics CSV and time selection sort
```

## Why This Was an Improvement

Earlier versions made the plugin drop too dependent on extra support decisions without clearly separating the algorithm modules from the data adapter.

The current split is better because it separates three concerns:

```text
ElectronicsSortSupport
    data preparation + comparator + timing harness

InsertionSort
    insertion sort mechanics + command registration

SelectionSort
    selection sort mechanics + command registration
```

This is a useful intermediate milestone because additional electronics algorithms can now be added with less duplication.

## Known Limitation: Still Too Electronics-Specific

The current design is still not the final plugin spine.

The names and contracts are electronics-specific:

```text
ElectronicsSortSupport
ElectronicsSortFunction
electronicsTargetLess
runElectronicsSortCommand
Load electronics CSV and time ...
```

The algorithms also still depend on the electronics comparator directly:

```cpp
electronicsTargetLess(left, right)
```

That means the current design supports:

```text
one electronics dataset
multiple electronics-aware sorting modules
```

The desired next design should support:

```text
whatever dataset is queued
whatever sorting algorithms are registered
```

## Required Comment Style

New nontrivial functions should keep the project’s comment shape:

```cpp
/*
 * Purpose:
 * Design:
 * Workflow:
 * Data Handoff:
 */
```

Use the comments to explain why the function exists, how it is designed, what steps it performs, and how data enters or leaves the module.

Avoid comments that are longer than the code they describe.

## TODO: Make Sorting Generic

The next refactor should remove electronics from the sort contract.

### Rename the support layer

Replace:

```text
ElectronicsSortSupport.h
ElectronicsSortSupport.cpp
```

with something like:

```text
SortSupport.h
SortSupport.cpp
```

or:

```text
DatasetSortSupport.h
DatasetSortSupport.cpp
```

### Introduce a generic comparator type

Replace:

```cpp
using ElectronicsSortFunction = void (*)(std::vector<Target>&);
```

with a comparator-aware shape:

```cpp
using TargetLess = std::function<bool(const Target&, const Target&)>;
using SortFunction = std::function<void(std::vector<Target>&, const TargetLess&)>;
```

Then algorithms call:

```cpp
less(left, right)
```

instead of:

```cpp
electronicsTargetLess(left, right)
```

### Introduce dataset work items

Create a queued dataset type that carries both records and ordering policy:

```cpp
struct QueuedDataset
{
    std::string name;
    std::string sourcePath;
    TargetList records;
    TargetLess less;
};
```

A dataset is more than one `Target`, so do not overload `TargetQueue` for this job. `TargetQueue` demonstrates FIFO behavior for individual `Target` records. A dataset queue should queue whole datasets.

Possible class:

```cpp
class DatasetQueue
{
public:
    static DatasetQueue& instance();

    void enqueue(QueuedDataset dataset);
    bool isEmpty() const;
    std::size_t size() const;
    QueuedDataset dequeue();
};
```

### Preserve the assignment data flow

When possible, dataset loaders should parse data into a linked-list structure first, then take an array snapshot for sorting.

Preferred generic flow:

```text
dataset plugin
-> parse file
-> TargetList
-> DatasetQueue
-> generic sort runner
-> TargetList::toVector()
-> std::vector<Target> array copies
-> registered sorting algorithms
-> sorted TargetList result
```

This keeps the linked list central while still allowing array-based algorithm timing.

### Introduce a sort registry

Sorting algorithms should register themselves independently from datasets.

Possible type:

```cpp
struct SortAlgorithm
{
    std::string name;
    SortFunction sort;
};

class SortRegistry
{
public:
    static SortRegistry& instance();

    bool registerAlgorithm(SortAlgorithm algorithm);
    std::vector<SortAlgorithm> algorithms() const;
};
```

Then `InsertionSort.cpp` and `SelectionSort.cpp` register algorithms, not dataset-specific menu commands.

### Separate dataset plugins from algorithm plugins

Dataset plugin responsibilities:

```text
know file path
parse file format
map rows into Target records
provide dataset-specific comparator
queue the prepared dataset
```

Algorithm plugin responsibilities:

```text
register algorithm name
sort std::vector<Target>& using supplied comparator
avoid file I/O
avoid dataset-specific logic
avoid console reporting
```

Generic runner responsibilities:

```text
dequeue one dataset
get all registered algorithms
clone arrays before timing
time only sort calls
report results
write selected sorted result back to TargetProgram::list()
```

### Add one generic menu command

Instead of one menu command per dataset/algorithm pair, add one generic runner command:

```text
Sort next queued dataset with available algorithms
```

This prevents an `N datasets x M algorithms` menu explosion.

Dataset plugins can still register queue commands:

```text
Queue electronics dataset
Queue messages dataset
Queue websites dataset
```

Algorithm plugins should not create user-facing commands unless there is a specific reason.

### Generic target ordering fallback

Add a default comparator for datasets that do not provide a custom one:

```text
compare fieldOne
then compare fieldTwo
```

Electronics can keep its natural reference-designator comparator as a dataset-provided policy.

### Timing rules for the generic runner

Keep these rules:

```text
Do not time file I/O.
Do not time CSV parsing.
Do not time linked-list loading.
Do not time vector cloning.
Do not time console output.
Only time calls to the sort function.
Use repetitions for small datasets.
Report total seconds and average seconds.
```

### Example future flow

```text
1. User chooses: Queue electronics dataset.
2. Electronics dataset plugin parses CSV into TargetList.
3. DatasetQueue stores the queued work item.
4. User chooses: Sort next queued dataset with available algorithms.
5. Generic sort runner dequeues the dataset.
6. SortRegistry supplies insertion sort, selection sort, and future algorithms.
7. Runner times each algorithm using the dataset's comparator.
8. Runner writes the sorted result back into TargetProgram::list().
```

## Current Milestone Summary

The current electronics support split is a good intermediate design:

```text
shared support module
+ thin algorithm modules
+ plugin menu registration
+ no changes to TargetList
+ no electronics fields added to Target
```

The next design move is to replace the electronics-specific support contract with a generic queued dataset and registered sort-algorithm system.
