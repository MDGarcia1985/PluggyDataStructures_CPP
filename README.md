# PluggyDataStructure

PluggyDataStructure is a C++17 learning project that loads TXT and CSV datasets into a family of classic data structures: a doubly linked list, stack, queue, binary search tree, integer-id graph, and separate-chaining hash table. Its UI, registries, sessions, operations, sorting support, file loading, and core data structures are separated so new structures and plugins can be added by reusing one generic registry plus menu-controller spine.

## Build And Run

### CMake (recommended)

CMake compiles every source file (including self-registering operation modules) and wires up the test suite for CTest:

For single-configuration generators such as Makefiles or Ninja:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/PluggyDataStructure
```

With the default Visual Studio generator on Windows, specify the configuration and use its configuration subdirectory:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
.\build\Debug\PluggyDataStructure.exe
```

Operation, command, and sort modules register themselves through global initializers. The build compiles them into an OBJECT library that is linked directly into both executables, so the linker never drops those object files.

### g++ (PowerShell)

From the project root:

```powershell
$sources = Get-ChildItem -Path src -Recurse -Filter *.cpp |
  ForEach-Object { $_.FullName }

g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude main.cpp $sources -o PluggyDataStructure.exe
.\PluggyDataStructure.exe
```

Build and run tests:

```powershell
$sources = Get-ChildItem -Path src -Recurse -Filter *.cpp |
  ForEach-Object { $_.FullName }
$tests = Get-ChildItem -Path tests -Recurse -Filter *.cpp |
  ForEach-Object { $_.FullName }

g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude $tests $sources -o ApplicationTests.exe
.\ApplicationTests.exe
```

### g++ (Bash)

From Git Bash, MSYS2 Bash, Linux, or macOS:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude main.cpp \
  $(find src -name '*.cpp') \
  -o PluggyDataStructure
./PluggyDataStructure

g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude \
  $(find tests -name '*.cpp') $(find src -name '*.cpp') \
  -o ApplicationTests
./ApplicationTests
```

PowerShell uses a backtick for line continuation, not `\`. The commands above avoid PowerShell continuation characters entirely except for the pipeline, where the line may naturally continue after `|`.

The VS Code tasks in `.vscode/tasks.json` configure, build, and test through CMake.

## Data Structures

After choosing a dataset the application opens a Data Structure Menu:

```text
Linked List  -> browse, add, delete, find, sort
Stack        -> push, pop, peek (LIFO)
Queue        -> enqueue, dequeue, peek (FIFO)
Binary Tree  -> insert, find, remove, in/pre/post/level-order traversals, height
Graph        -> nodes, adjacency, BFS, DFS, add edge
Hash Table   -> insert/update, find, erase, bucket view, load factor
```

Each structure is populated from the currently selected dataset. The graph also reads an optional edge-list companion file (see Dataset Support).

## Dataset Support

The startup flow scans `data/` and displays all regular files in filename order.

```text
.txt -> plain text/list loader
.csv -> structured table loader
other -> displayed as unsupported
```

TXT files accept either `fieldOne|fieldTwo` records or ordinary one-item lines. CSV files use the first row as headings, map the first column to `Target::fieldOne()`, and store labeled remaining values in `fieldTwo()`.

All structure sessions use the same loading policy. If the selected TXT or CSV file cannot be opened or contains no usable records, `FileLoader::loadTargetsOrFallback()` supplies the built-in 20-record dataset before the selected structure is constructed.

### Graph edge files

The graph reuses any node dataset and looks for a companion edge file named `<dataset-base>.edges`. For example, selecting `data/social.csv` loads edges from `data/social.edges`.

```text
# comment lines start with '#'
# a line containing only "#directed" makes following edges directed
fromKey|toKey|weight     # weight optional, defaults to 1.0, undirected by default
fromKey,toKey,weight     # comma-delimited rows are also accepted
```

Edges reference nodes by their first field (`Target::fieldOne()`). If no edge file exists, the graph still loads the nodes and edges can be added from the menu.

Edge loading reports distinct `NotFound`, `Loaded`, `Empty`, and `Invalid` outcomes. Invalid rows are counted, valid rows can still load from a partially malformed file, and edge records whose node keys do not exist are reported separately.

## Menu Flow

```text
MainMenu
-> DataSourceMenu
-> StructureMenu (Linked List / Stack / Queue / Tree / Graph / Hash Table)
-> MenuController<Registry, Session>
-> registered operation
-> (Linked List) optional SortTypeMenu -> registered sorting algorithm
```

`Menu` is generic. It only displays string labels, prompts for numbers, validates ranges, returns selected positions, and prints the shared invalid-selection message.

Menu-specific decisions live in dedicated controllers:

- `MainMenu` starts the application flow.
- `DataSourceMenu` discovers and classifies datasets.
- `StructureMenu` chooses a data structure and builds its session.
- `MenuController<RegistryT, SessionT>` renders any registry's operations and runs the selected one against the active session.
- `SortTypeMenu` executes registered sorting algorithms for the linked list.

## Generic Registry And Menu Spine

Every data structure reuses the same four-part pattern:

```text
RegistryBase<Item>          owns shared item storage
OperationRegistry<Session>  adds validation, duplicate policy, Exit policy, ordering, lookup
MenuController<Reg, Session> converts operations to labels and runs the selected one
XSession                    owns one structure instance plus its load context
```

Concrete registries (`CommandRegistry`, `SortRegistry`, and the `StructureRegistry<Session>` aliases `StackRegistry`, `QueueRegistry`, `TreeRegistry`, `GraphRegistry`, `HashTableRegistry`) layer their policy on the shared base.

Command and structure operations require positive, unique IDs. ID `0` is reserved for registry-owned Exit/Back entries. Sort entries are identified by unique labels because their registration-order IDs remain `0`.

## Plugin Registration

Linked-list commands are independent source files in `src/commands/`, each registering with `CommandRegistry`:

```cpp
PDS_REGISTER_COMMAND(9, "New command", pds::newCommand)
```

Exit commands use `PDS_REGISTER_EXIT_COMMAND`. `CommandRegistry` orders regular commands by ID and always moves Exit to the end.

Sorting algorithms register with `SortRegistry`:

```cpp
PDS_REGISTER_SORT("Algorithm Name", pds::algorithmCommand)
```

Operations for the other structures live in `src/operations/` and register with their structure registry through the generic macro:

```cpp
using TreeOp = pds::Operation<pds::TreeSession>;
PDS_REGISTER_OPERATION(pds::TreeRegistry::instance(),
    TreeOp{9, "New tree operation", [](pds::TreeSession& session) { /* ... */ }})
```

Each `StructureRegistry` seeds its own ID `0` Exit/Back item, rejects duplicate operation IDs and labels, and returns Exit/Back last. Adding an operation never requires editing a menu.

## Project Layout

```text
include/
  app/        App.h
  core/       Header.h, Target.h, TargetList.h, TargetProgram.h,
              TargetStack.h, TargetQueue.h, TargetTree.h,
              TargetGraph.h, TargetHashTable.h, VisitedSet.h
  io/         FileLoader.h
  registry/   RegistryBase.h, Operation.h, OperationRegistry.h,
              CommandRegistry.h, SortRegistry.h, StructureRegistries.h
  session/    StackSession.h, QueueSession.h, TreeSession.h,
              GraphSession.h, HashTableSession.h
  sorting/    SortSupport.h
  ui/         Display.h, Menu.h, MenuController.h, MainMenu.h,
              DataSourceMenu.h, StructureMenu.h,
              TargetDataStructureMenu.h, SortTypeMenu.h

src/
  app/        App.cpp
  commands/   one file per linked-list command
  algorithms/ InsertionSort.cpp, SelectionSort.cpp
  operations/ StackOperations.cpp, QueueOperations.cpp, TreeOperations.cpp,
              GraphOperations.cpp, HashTableOperations.cpp
  core/       Target.cpp, TargetList.cpp, TargetProgram.cpp,
              TargetStack.cpp, TargetQueue.cpp, TargetTree.cpp,
              TargetGraph.cpp, TargetHashTable.cpp
  io/         FileLoader.cpp
  registry/   CommandRegistry.cpp, SortRegistry.cpp
  session/    StackSession.cpp, QueueSession.cpp, TreeSession.cpp,
              GraphSession.cpp, HashTableSession.cpp
  sorting/    SortSupport.cpp
  ui/         Display.cpp, Menu.cpp, MainMenu.cpp, DataSourceMenu.cpp,
              StructureMenu.cpp, TargetDataStructureMenu.cpp, SortTypeMenu.cpp

tests/
  TestHarness.h, TestMain.cpp, ApplicationTests.cpp,
  TargetTreeTests.cpp, TargetGraphTests.cpp, TargetHashTableTests.cpp
```

## Tests

Tests self-register with `PDS_TEST(name)` into a shared `TestRegistry` (mirroring the application's plugin spine). `TestMain.cpp` runs every registered case, prints a pass/fail/assertion summary, and returns a nonzero exit code on failure for CI.

## Extension Points

Add a data structure by following the existing quartet: a `core/` structure, an `XSession`, an `XRegistry` alias of `StructureRegistry<XSession>`, and one `src/operations/` file. Route it from `StructureMenu` with `MenuController<XRegistry, XSession>`. No changes to `Menu` or the generic spine are required.

New and modified functions use the standard `Purpose`, `Design`, `Workflow`, and `Data Handoff` header.
