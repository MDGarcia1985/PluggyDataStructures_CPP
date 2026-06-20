# DEVNOTES

## Purpose

This file records the design decisions made while extending `LinkedListBrowser` from a generic linked-list browser into a plugin-driven data-structure test bed. The notes stop at the current milestone achieved and next steps planned.

The file is maintained chronologically. Earlier references to "current" behavior describe the state at that historical milestone; later completed-milestone sections supersede them without deleting the decisions that led there.


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

## Completed Milestone: Generic Dataset And Sort Menus

Date completed:

```text
2026-06-13
```

The portability refactor described above has now been implemented. The earlier electronics-specific design remains documented because it explains how the project reached the current architecture.

The implementation followed the later application specification rather than every detail of the earlier queued-dataset proposal. In particular, the application now sorts the dataset currently selected by the user instead of introducing a separate `DatasetQueue`.

Reason:

```text
The active TargetList already represents the selected work item.
The requested workflow opens a Sort Type Menu for that active list.
A second queue would add state and ownership complexity without serving the current UI.
```

### Completed: Rename the support layer

The former files:

```text
ElectronicsSortSupport.h
ElectronicsSortSupport.cpp
```

were renamed and redesigned as:

```text
SortSupport.h
SortSupport.cpp
```

All includes, symbols, comments, tests, and root documentation were updated. Shared sorting support no longer assumes that records describe electronics.

### Completed: Generic dataset discovery

`FileLoader::discoverDataFiles()` now scans `data/` for regular files and returns them in filename order.

The startup menu is generated from the discovered files instead of hardcoding:

```text
data/websites.txt
data/messages.txt
```

Unsupported files remain visible and are labeled as unsupported. Selecting one reports the error and redisplays the data file menu.

### Completed: Extension-based loading

The selected extension determines the loader:

```text
.txt -> plain text/list loader
.csv -> structured table loader
other -> unsupported
```

Extension matching is case-insensitive.

TXT decisions:

```text
pipe-delimited rows preserve the existing fieldOne|fieldTwo format
ordinary non-comment lines are accepted as one-field list items
blank lines and # comments are skipped
```

CSV decisions:

```text
the first non-empty row supplies column headings
the first data column becomes Target::fieldOne
remaining non-empty columns become labeled Target::fieldTwo attributes
quoted commas and doubled quote escapes are supported
```

This replaces the earlier electronics-column mapping with a table mapping that can represent any standard CSV dataset.

### Completed: Generic sorting contract

The active contract is:

```cpp
using SortFunction = void (*)(std::vector<Target>&);

bool targetLess(const Target& left, const Target& right);

void runSortCommand(
    TargetProgram& program,
    const std::string& algorithmName,
    SortFunction sortFunction);
```

The comparator is generic and case-insensitive:

```text
compare fieldOne
then compare fieldTwo
then use the original display text as a stable tie-breaker
```

The earlier proposal considered passing a comparator into each algorithm. The implemented algorithms call the shared `targetLess()` function instead.

Reason:

```text
The current application defines one generic ordering policy for Target records.
Keeping that policy in SortSupport still prevents algorithm duplication.
The contract can be expanded later if datasets require selectable custom ordering.
```

### Completed: Sort registry

`SortRegistry` is now independent from the main `CommandRegistry`.

Each sorting module registers a `SortCommand` containing:

```text
display label
action receiving TargetProgram&
```

The current modules register:

```text
Selection Sort
Insertion Sort
```

Future algorithms can register with:

```cpp
LLB_REGISTER_SORT("Algorithm Name", pds::algorithmCommand)
```

No Sort Type Menu rewrite is required.

### Completed: Thin algorithm modules

`InsertionSort.cpp` and `SelectionSort.cpp` now contain only:

```text
algorithm logic
small command adapter
sort registration
```

They do not:

```text
discover files
load TXT or CSV data
parse rows
choose timing repetitions
replace linked-list nodes directly
render menus
print the timing report
```

Those shared responsibilities are owned by `FileLoader`, `Menu`, and `SortSupport`.

### Completed: Sort the active dataset

The main menu now contains:

```text
7) Sort list
```

Selecting it opens the generated Sort Type Menu. The chosen algorithm receives a vector snapshot of the active `TargetList`.

The sorting workflow is:

```text
selected data file
-> FileLoader
-> active TargetList
-> Sort list command
-> Sort Type Menu
-> registered sort command
-> TargetList::toVector()
-> timed vector copies
-> sorted TargetList replacement
```

This preserves the linked list as the application’s central data structure while using arrays for algorithm timing.

### Completed: Timing boundaries

The earlier timing rules were retained:

```text
file discovery is not timed
file I/O is not timed
TXT or CSV parsing is not timed
linked-list loading is not timed
vector cloning is not timed
console output is not timed
only calls to the sort function are timed
```

Small datasets are sorted repeatedly. The application reports total elapsed seconds and average seconds per sort.

### Completed: Menu validation and Exit ordering

All interactive menus validate that the entered number matches a displayed option.

Invalid entries report:

```text
Invalid selection. Please choose one of the listed options.
```

The relevant menu is then redisplayed.

The main command registry now marks the Exit command explicitly. Visible menu numbers are generated from command positions rather than registration IDs.

Ordering rule:

```text
regular registered commands appear first
Exit is appended last
```

The Sort Type Menu follows the same Exit-last rule.

### Completed: Function documentation

Every newly created or modified function uses the required header:

```cpp
/*
 * Purpose:
 * Design:
 * Workflow:
 * Data Handoff:
 */
```

### Validation completed

The completed refactor was checked with:

```text
C++17 application build with -Wall -Wextra -pedantic
C++17 test build with -Wall -Wextra -pedantic
111 passing automated tests
scripted interactive TXT and CSV selection
invalid dataset, main-menu, and sort-menu entries
registered sorting
Exit-last behavior
```

### Current extension points

Add a dataset:

```text
Place a standard .txt or .csv file in data/.
```

Add a sorting algorithm:

```text
Create an algorithm module.
Implement the vector sort.
Add a small command adapter calling runSortCommand().
Register it with LLB_REGISTER_SORT.
```

The historical queued-dataset design remains a possible future direction if the application later needs to hold and schedule multiple datasets simultaneously. It is not required by the current single-active-dataset workflow.

## Completed Milestone: Menu And Registry Architecture Refactor

Date completed:

```text
2026-06-13
```

The menu and registration architecture was reorganized after the generic dataset and sorting milestone. This section records the completed changes without replacing the earlier decisions that led to them.

### Completed: Ownership-based directories

Flat headers and mixed source folders were moved into:

```text
include/app
include/core
include/io
include/registry
include/sorting
include/ui

src/app
src/commands
src/algorithms
src/core
src/io
src/registry
src/sorting
src/ui
```

Include paths now state module ownership explicitly, such as:

```cpp
#include "core/TargetProgram.h"
#include "registry/CommandRegistry.h"
#include "ui/SortTypeMenu.h"
```

### Completed: Generic Menu

`Menu` no longer contains:

```text
CommandPlugin
CommandRegistry
registration macros
dataset discovery
file type decisions
target menu headings
sort menu behavior
```

Its active responsibilities are:

```text
display a title and string labels
prompt for numeric input
validate option ranges
return a zero-based selection
print the shared invalid-selection message
redisplay complete menus after invalid choices
```

Reason:

```text
A reusable menu should not know what an option means or where options come from.
```

### Completed: Dedicated menu controllers

The application flow is now separated into:

```text
MainMenu
DataSourceMenu
TargetDataStructureMenu
SortTypeMenu
```

`MainMenu` starts the application flow.

`DataSourceMenu` owns file discovery labels, unsupported-file reporting, and the selected path.

`TargetDataStructureMenu` obtains target/list commands from `CommandRegistry` and executes the selected action.

`SortTypeMenu` obtains algorithms and Exit from `SortRegistry` and executes the selected action.

### Completed: Shared registry base

`RegistryBase<Item>` was added to centralize owned registration storage.

Derived registries apply their own validation and ordering rules before or after using the shared storage.

This was intentionally kept small. It avoids forcing command and sort entries into an artificial identical schema while still removing duplicated container ownership behavior.

### Completed: CommandRegistry extraction

`CommandRegistry` moved from `Menu.h/cpp` into:

```text
include/registry/CommandRegistry.h
src/registry/CommandRegistry.cpp
```

It continues to:

```text
reject invalid registrations
reject duplicate IDs
allow one Exit command
sort regular commands by ID
return Exit last
find commands by stable ID
```

The command registration macros moved with the registry contract.

### Completed: SortRegistry extraction

`SortRegistry` moved out of `SortSupport` into:

```text
include/registry/SortRegistry.h
src/registry/SortRegistry.cpp
```

The registry now owns the Sort Type Menu's Exit item. Algorithms register only executable sorting entries. `SortTypeMenu` therefore receives every displayed item from the registry instead of appending an Exit label itself.

### Completed: Individual command modules

Target/list command adapters moved out of `TargetProgram.cpp` into:

```text
DisplayListCommand.cpp
MoveForwardCommand.cpp
MoveBackwardCommand.cpp
AddTargetCommand.cpp
DeleteTargetCommand.cpp
FindTargetCommand.cpp
SortListCommand.cpp
ExitCommand.cpp
```

`SortListCommand.cpp` was added after confirming that Sort list should follow the same one-command-per-file rule as the other target commands.

Each command module contains:

```text
required includes
one small action adapter
one registration declaration
```

### Completed: TargetProgram simplification

`TargetProgram.cpp` no longer:

```text
defines command adapters
registers commands
discovers datasets
renders the target command menu
dispatches command registry entries
implements runApp()
```

It retains active list state and business operations. `App.cpp` now passes the program into `MainMenu`, so core code does not start UI navigation.

Application startup moved to:

```text
include/app/App.h
src/app/App.cpp
```

### Completed: SortSupport simplification

`SortSupport` no longer owns:

```text
SortRegistry implementation
Sort Type Menu rendering
sort menu validation
Exit menu behavior
```

It retains:

```text
generic Target ordering
timing policy
sort execution
sorted-list replacement
timing output
```

This keeps sorting support separate from both algorithm registration and UI navigation.

### Completed: Build and validation updates

The VS Code `cl.exe` task now lists all source files in the reorganized folders.

Validation completed:

```text
C++17 application build with -Wall -Wextra -pedantic
C++17 test build with -Wall -Wextra -pedantic
113 passing automated tests
dynamic CSV dataset selection
invalid dataset menu redisplay
invalid target menu redisplay
invalid sort menu redisplay
CommandRegistry Exit-last behavior
SortRegistry Exit-last behavior
Sort Type Menu return to the target menu
application Exit behavior
```

### Current extension direction

Future data structures can reuse `Menu` while defining independent controllers and registries:

```text
HashTableMenu and HashTableRegistry
TreeMenu and TreeRegistry
GraphMenu and GraphRegistry
SearchMenu and SearchRegistry
```

This milestone establishes the folder and dependency pattern those systems can follow.

## Completed Milestone: Generic Structures Framework + Tree, Graph, Hash Table

Date completed:

```text
2026-06-13
```

The data-structure menus described as future work were implemented on a single reusable spine instead of copying the command/sort registries per structure.

### Generic spine

```text
RegistryBase<Item>             owns shared item storage (unchanged)
Operation<SessionT>            id, label, action(SessionT&), isExit
OperationRegistry<SessionT>    validation, duplicate policy, Exit ordering, findById
MenuController<RegistryT, SessionT>  label adapter + execution loop
```

`CommandRegistry` and `SortRegistry` were migrated onto this spine. `CommandPlugin` and `SortCommand` are now aliases of `Operation<TargetProgram>`, so existing command and sort modules, macros, and tests were preserved unchanged. `TargetDataStructureMenu` became a thin wrapper over `MenuController`.

### Structure registries

`StructureRegistry<SessionT>` is one template that seeds its own Exit/Back item, rejects duplicate labels, and exposes Exit-last ordering. The named registries are aliases:

```text
StackRegistry, QueueRegistry, TreeRegistry, GraphRegistry, HashTableRegistry
```

### Sessions and operations

Each structure has an `XSession` (owns the structure plus interactive I/O) and an `src/operations/XOperations.cpp` module that registers its operations with `LLB_REGISTER_OPERATION`. A new top-level `StructureMenu` builds the chosen session from the selected dataset and runs `MenuController<XRegistry, XSession>`.

Deviation from the plan: operations are grouped one file per structure (for example `TreeOperations.cpp`) rather than one file per operation. The registry/menu pattern is identical; this keeps the change reviewable. Splitting into one-op-per-file later is mechanical.

### Core structures

```text
TargetList        gained O(1) front/back/removeFront/removeBack; stack/queue now O(1)
TargetTree        BST keyed by targetLess; rule-of-five; iterative destroy;
                  insert/find/remove; in/pre/post/level-order traversals; height
TargetGraph       integer-id arena + adjacency list; BFS (queue) and DFS (stack);
                  VisitedSet (vector<bool>) prevents revisits in cyclic graphs
TargetHashTable   separate chaining backed by TargetList; case-insensitive key;
                  rehash when load factor > 0.75; bucket views for display
```

### Data input

Tree, stack, queue, and hash sessions load from the selected TXT/CSV dataset. The graph also reads an optional `<dataset-base>.edges` companion file (`from|to|weight`, undirected by default, `#directed` directive supported). `FileLoader::discoverEdgeFile()` and `FileLoader::loadEdges()` were added, plus the sample `data/social.csv` and `data/social.edges`.

### Tests and build

The test suite became a self-registering harness: `LLB_TEST(name)` registers into a shared `TestRegistry`, and `TestMain.cpp` runs all cases, prints a summary, and returns a CI-friendly exit code. New per-structure test files cover the tree, graph, and hash table.

A `CMakeLists.txt` was added. Registration modules are compiled into an OBJECT library that is linked directly into both executables so the linker cannot drop the global initializers that drive self-registration. CTest runs the suite. The VS Code tasks now configure, build, and test through CMake.

### Validation completed

```text
C++17 application build with -Wall -Wextra -pedantic (clean)
C++17 test build with -Wall -Wextra -pedantic (clean)
23 test cases, 208 assertions, all passing
```

## Completed Milestone: Shared Fallback Loading And Unambiguous Structure IDs

Date completed:

```text
2026-06-15
```

This milestone follows the generic structures framework and addresses review findings without rewriting the historical decisions above.

### Completed: One fallback policy for every structure

Previously, `TargetProgram::loadInitialData()` loaded the built-in fallback dataset when the selected file failed, while `StructureMenu` loaded stack, queue, tree, graph, and hash-table inputs directly and silently constructed empty sessions on the same failure.

`FileLoader::loadTargetsOrFallback()` now owns the shared policy:

```text
clear caller-owned records
try the selected TXT or CSV file
return true when selected data loads
otherwise load the built-in 20-record dataset
return false to report that fallback was used
```

Both `TargetProgram` and `StructureMenu` use this entry point. Every structure therefore receives the same data for a selected path, including missing and empty-file cases.

An integration test drives `StructureMenu` with scripted input, selects the stack for a missing dataset, displays its contents, and verifies records from both ends of the fallback collection.

### Completed: Positive and unique structure operation IDs

`StructureRegistry<SessionT>` now enables the same positive-ID and duplicate-ID checks used by command registration, while retaining unique-label validation.

The active invariant is:

```text
ID 0 is reserved for the registry-owned Back item
registered structure operation IDs must be positive
registered structure operation IDs must be unique within that registry
registered structure operation labels must be unique within that registry
findById(id) identifies at most one operation
```

Regression tests reject a duplicate tree-operation ID and a negative ID, then verify only one operation owns the tested ID.

### Documentation update

`README.md` and `ARCHITECTURE.md` now describe:

```text
StructureMenu as the live post-dataset routing layer
the per-structure session/registry/controller branches
shared selected-file-or-fallback loading
the ID 0 Back reservation and positive unique operation IDs
the current graph edge diagnostic limitation
```

The earlier sections of this file remain unchanged as historical records. This dated section supersedes their obsolete current-state descriptions.

### Reviewed: graph edge-file diagnostics

No edge-loading behavior changed in this milestone. The review confirmed that `StructureMenu` currently uses an empty edge vector to decide that no companion file was found. That wording is inaccurate when a companion file exists but is empty or contains no valid rows.

Recommended next design:

```text
EdgeLoadResult
  status: NotFound | Loaded | Empty | Invalid
  edges: vector<EdgeRecord>
  skippedRowCount
```

`GraphSession` can additionally report how many parsed edges were rejected because their endpoint keys do not exist in the node dataset.

Recommended messages:

```text
NotFound -> No companion edge file found; graph loaded with nodes only.
Empty    -> Companion edge file contains no edge records.
Invalid  -> Companion edge file contains no valid edge records; report skipped rows.
Loaded   -> Load edges and optionally report unresolved endpoint references.
```

This result object is preferred over inferring file state from `edges.empty()` because an empty graph is valid and should remain usable.

### Validation completed

```text
C++17 application build with -Wall -Wextra -pedantic (clean)
C++17 test build with -Wall -Wextra -pedantic (clean)
24 test cases, 213 assertions, all passing
```

## Completed Milestone: Structured Graph Edge Diagnostics

Date completed:

```text
2026-06-15
```

This milestone implements the graph edge diagnostic design recommended in the immediately preceding milestone.

### Completed: EdgeLoadResult

`FileLoader::loadEdges()` now returns:

```text
EdgeLoadResult
  status: NotFound | Loaded | Empty | Invalid
  edges: vector<EdgeRecord>
  skippedRowCount
```

Status meanings:

```text
NotFound -> the companion path cannot be opened
Loaded   -> at least one valid edge record was parsed
Empty    -> the file contains no data rows after comments, directives, and headers
Invalid  -> data rows exist, but none are valid edge records
```

A partially malformed file remains `Loaded`; valid records are retained and invalid rows contribute to `skippedRowCount`.

### Completed: Endpoint-resolution diagnostics

`GraphSession` now counts parsed edge records that cannot be added because either endpoint key is absent from the loaded graph nodes.

The responsibilities remain separate:

```text
FileLoader   -> file state, syntax parsing, skipped malformed rows
GraphSession -> node-key resolution and unresolved endpoint count
StructureMenu -> user-facing diagnostic messages
```

This avoids coupling generic file parsing to a particular graph instance.

### Completed: Accurate graph messages

`StructureMenu` now distinguishes:

```text
missing companion file
empty companion file
file containing only invalid rows
partially valid file with skipped rows
parsed edges whose endpoint keys are unresolved
```

All outcomes still produce a usable graph session. Missing, empty, or invalid edge files load the selected nodes without edges.

### Validation completed

```text
C++17 application build with -Wall -Wextra -pedantic (clean)
C++17 test build with -Wall -Wextra -pedantic (clean)
25 test cases, 222 assertions, all passing
```

### CMake validation

The CMake workflow was subsequently verified with CMake 4.4.0-rc1 and the Visual Studio 18 2026 generator:

```text
configure succeeded
Debug application and test builds succeeded
CTest passed unit_tests
```

The Windows documentation now includes `--config Debug`, `ctest -C Debug`, and the generated `build/Debug/LinkedListBrowser.exe` path required by Visual Studio's multi-configuration layout.

## Completed Milestone: Map Word Frequency Counter And Documentation Audit

Date completed:

```text
2026-06-20
```

This milestone adds the map-focused module as a first-class structure system and records the repository-wide documentation revision.

### Completed: First-class map structure

The word-frequency feature follows the same structure spine as stack, queue, tree, graph, and hash table:

```text
StructureMenu
-> MenuController<MapRegistry, MapSession>
-> registered MapOperations
-> MapSession
-> TargetMap
-> std::map<std::string, int>
```


### Completed: TargetMap ownership

`TargetMap` owns the actual ordered word-frequency storage:

```cpp
std::map<std::string, int> wordCounts_;
```

Its responsibilities are:

```text
replace counts from supplied text
replace counts from both fields of loaded Targets
split words at punctuation boundaries
normalize uppercase letters to lowercase
maintain total and unique counts
return alphabetically ordered read-only frequencies
report maximum frequency and all ties
clear state and report emptiness
```

The parser uses safe unsigned-character casts with `std::isalnum` and `std::tolower`. A compound such as `hello-world` is counted as `hello` and `world`; case variants share one normalized key.

`TargetMap` does not read from `std::cin`, write to `std::cout`, or depend on menu/display classes, so its behavior is directly testable.

### Completed: Map session and operations

`MapSession` owns one `TargetMap` and a snapshot of the selected dataset. It handles full-line typed input, user messages, frequency-table rendering, tied-most-frequent output, summary output, and empty-state guards.

`MapOperations.cpp` registers:

```text
1 Count words from typed text
2 Count words from loaded target data
3 Show word frequencies
4 Show most frequent word(s)
5 Show word count summary
6 Clear word counts
```

`MapRegistry` supplies the ID `0` Back operation and keeps it last. `StructureMenu` now presents Map / Word Frequency Counter as the seventh structure choice and Exit as the eighth choice.

### Completed: Documentation revision

Tracked C++ files were audited for the standard file banner:

```text
File
Description
Copyright
Contact
Site
SPDX-License-Identifier
```

New and previously undocumented function bodies were documented using:

```text
Purpose
Design
Workflow
Data Handoff
```

Test functions and inline helper functions touched by the audit follow the same contract. The established `Purpose` spelling was retained consistently.

### Completed: Tests and validation

`TargetMapTests.cpp` covers:

```text
case-insensitive word counting
punctuation and compound splitting
alphabetical map iteration
total and unique counts
most-frequent ties
Target field analysis
replacement and clear behavior
MapSession preload integration
MapRegistry operation wiring
```

Validation completed:

```text
C++17 GCC 15.2 build with -Wall -Wextra -pedantic (clean)
29 test cases, 240 assertions, all passing
tracked C++ file-header audit passed
git diff --check passed
```
