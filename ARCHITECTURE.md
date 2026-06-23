# Architecture

PluggyDataStructure separates reusable UI behavior, menu navigation, registration, business operations, data loading, and sorting.

## Runtime Flow

```text
main.cpp
-> app/App.cpp
-> MainMenu
-> DataSourceMenu
-> FileLoader::loadTargetsOrFallback
-> StructureMenu
   -> Linked List: TargetProgram -> CommandRegistry -> MenuController
      -> optional SortTypeMenu -> SortRegistry action -> SortSupport
   -> Stack: StackSession -> StackRegistry -> MenuController
   -> Queue: QueueSession -> QueueRegistry -> MenuController
   -> Tree: TreeSession -> TreeRegistry -> MenuController
   -> Graph: GraphSession -> GraphRegistry -> MenuController
      -> optional companion edge file through FileLoader
   -> Hash Table: HashTableSession -> HashTableRegistry -> MenuController
   -> Map / Word Frequency Counter: MapSession -> MapRegistry -> MenuController
      -> TargetMap -> std::map<std::string, int>
```

`MainMenu` loads the selected dataset once to report its size. `StructureMenu` creates a fresh structure session for every selection and applies the same selected-file-or-fallback policy before construction.

`TargetProgram` owns the linked-list session's active `TargetList`, selected data path, and exit state. It implements list operations but does not register commands or render application menus.

## Generic Menu Boundary

`ui/Menu` knows only:

```text
menu title
string option labels
numeric input
valid option count
selected position
invalid-selection text
```

It has no dependency on file loading, linked-list commands, sorting, or registries.

The menu controllers own context:

- `MainMenu` coordinates startup.
- `DataSourceMenu` discovers files and applies extension rules.
- `StructureMenu` selects a structure and constructs its session.
- `MenuController<RegistryT, SessionT>` renders and dispatches registered operations.
- `TargetDataStructureMenu` remains a thin linked-list wrapper over `MenuController`.
- `SortTypeMenu` gets algorithms from `SortRegistry`.

## Registry Boundary

`RegistryBase<Item>` provides shared item storage and append/read behavior.

`CommandRegistry` adds:

```text
command field validation
duplicate ID rejection
single Exit enforcement
ID ordering
Exit-last ordering
ID lookup
```

`SortRegistry` adds:

```text
algorithm validation
duplicate label rejection
registry-owned Exit item
registration-order preservation
Exit-last ordering
```

`StructureRegistry<SessionT>` adds:

```text
positive operation ID validation
duplicate ID rejection
duplicate label rejection
registry-owned ID 0 Back item
ID ordering
Back-last ordering
unambiguous ID lookup
```

The registry headers own registration macros. `Menu.h` no longer contains plugin definitions or macros.

## Command Modules

Each file in `src/commands/` owns one target-menu action adapter and its registration:

```text
DisplayListCommand
MoveForwardCommand
MoveBackwardCommand
AddTargetCommand
DeleteTargetCommand
FindTargetCommand
SortListCommand
ExitCommand
```

The adapters call public `TargetProgram` operations. They do not own list state or navigation loops.

`SortListCommand` opens `SortTypeMenu`; it does not know which algorithms are registered.

## Sorting Boundary

Algorithm files contain:

```text
algorithm implementation
small TargetProgram adapter
SortRegistry registration
```

`sorting/SortSupport` owns:

```text
generic Target comparison
timing repetition policy
vector snapshots and copies
sorted TargetList replacement
timing output
```

`registry/SortRegistry` owns algorithm registration. `ui/SortTypeMenu` owns sorting navigation. Neither responsibility remains in `SortSupport`.

## Core Boundary

`core/TargetList` remains domain-neutral. It owns node memory, list mutation, navigation, search, and vector snapshots.

`core/TargetProgram` coordinates business operations on the active list:

```text
load selected path
display records
move current record
add record
delete record
find record
track exit state
```

`io/FileLoader` owns the common loading policy:

```text
try the selected TXT or CSV dataset
clear partial or previous records
load the built-in 20-record fallback when loading fails
hand the same resulting Targets to every structure path
```

Application startup passes it into `MainMenu`; core code does not start UI navigation.

## Dependency Direction

```text
core/Target
    ^
core/TargetList
    ^
core/TargetProgram <--- commands
    ^       ^              ^
    |       |              |
   io      ui <-------- registries
            ^
            |
         app/Main

algorithms -> SortRegistry
algorithms -> SortSupport -> TargetProgram
SortTypeMenu -> SortRegistry
```

`Menu` sits below menu controllers as a reusable console helper. Registries do not render menus. Core containers do not depend on registries. `TargetStack` and `TargetQueue` retain legacy `display()` convenience methods that delegate to `ui/Display`; interactive structure behavior otherwise lives in sessions.

## Include Paths

Includes reflect ownership:

```cpp
#include "app/App.h"
#include "core/TargetProgram.h"
#include "io/FileLoader.h"
#include "registry/CommandRegistry.h"
#include "registry/SortRegistry.h"
#include "sorting/SortSupport.h"
#include "ui/Menu.h"
#include "ui/SortTypeMenu.h"
```

Flat legacy include paths are no longer used.

## Generic Structures Spine

The registry and menu layers were generalized so every data structure reuses one pattern instead of copying the command/sort registries.

```text
RegistryBase<Item>                   shared item storage
Operation<SessionT>                  id, label, action(SessionT&), isExit
OperationRegistry<SessionT>          validation, duplicate policy, Exit ordering, lookup
MenuController<RegistryT, SessionT>   converts operations to labels and runs the selection
XSession                             owns one structure instance plus its load context
```

`CommandRegistry` and `SortRegistry` derive from `OperationRegistry<TargetProgram>`; `CommandPlugin` and `SortCommand` are aliases of `Operation<TargetProgram>`. `StructureRegistry<SessionT>` is a single template (aliased as `StackRegistry`, `QueueRegistry`, `TreeRegistry`, `GraphRegistry`, `HashTableRegistry`, `MapRegistry`) that seeds its own ID `0` Back item. Registered structure operations require positive IDs, unique IDs, and unique labels.

`StructureMenu` sits above the per-structure controllers:

```text
MainMenu -> DataSourceMenu -> StructureMenu -> MenuController<XRegistry, XSession>
```

## Implemented Structure Systems

```text
Linked List   TargetProgram + CommandRegistry + commands
Stack         StackSession + StackRegistry + StackOperations
Queue         QueueSession + QueueRegistry + QueueOperations
Binary Tree   TargetTree + TreeSession + TreeRegistry + TreeOperations
Graph         TargetGraph (integer-id arena) + VisitedSet + GraphSession + GraphRegistry + GraphOperations
Hash Table    TargetHashTable (separate chaining) + HashTableSession + HashTableRegistry + HashTableOperations
Map           TargetMap (ordered word counts) + MapSession + MapRegistry + MapOperations
```

Sessions own interactive I/O and operation modules in `src/operations/` self-register. Every session receives either the selected dataset or the shared fallback dataset. The graph reads an optional `<dataset>.edges` companion file via `FileLoader`.

## Map / Word Frequency Boundary

`core/TargetMap` owns word-frequency data and analysis. Its storage is:

```cpp
std::map<std::string, int> wordCounts_;
```

The ordered map keeps frequency output alphabetical without a second sorting pass. `TargetMap` has no console or menu dependencies. It:

```text
parses supplied text into alphanumeric words
splits punctuation-separated compounds such as hello-world
normalizes letters to lowercase
counts total and unique words
reports the maximum frequency and every tied word
counts both fields from a Target snapshot
clears and exposes frequencies through read-only access
```

`session/MapSession` owns interactive input and output plus the selected dataset snapshot. It replaces the current analysis when counting typed text or loaded Target fields, guards empty-state displays, and delegates all parsing and storage to `TargetMap`.

`operations/MapOperations.cpp` registers six positive-ID operations with `MapRegistry`:

```text
count typed text
count loaded Target fields
show alphabetical frequencies
show most-frequent word or ties
show summary metrics
clear counts
```

The registry-owned ID `0` Back item remains last in the generated menu.

## Documentation Convention

Tracked C++ source and header files carry a file banner containing:

```text
File
Description
Copyright
Contact
Site
SPDX-License-Identifier
```

Function implementations use the project comment contract:

```text
Purpose
Design
Workflow
Data Handoff
```

These comments describe ownership and movement of data without changing dependency direction.

## Graph Edge Diagnostics

`FileLoader::loadEdges()` returns an `EdgeLoadResult`:

```text
status: NotFound | Loaded | Empty | Invalid
edges: parsed EdgeRecord values
skippedRowCount: malformed data rows ignored by the parser
```

Parsing and graph resolution remain separate. `FileLoader` reports file and row state; `GraphSession` counts parsed edges rejected because a source or destination key is absent from the selected node dataset. `StructureMenu` combines both results into accurate user-facing diagnostics while keeping an edgeless graph usable.

## Build Note: Self-Registration

Operation, command, and sort modules register through global initializers. The CMake build compiles all `src/` files into an OBJECT library linked directly into both executables, ensuring the linker retains those object files so registration runs before `main`.

## Future Systems

The same spine supports further additions (for example a search system) by adding a `core/` structure, an `XSession`, an `XRegistry` alias, and one `src/operations/` module, then routing it from `StructureMenu`. No changes to `Menu` or the generic spine are required.
