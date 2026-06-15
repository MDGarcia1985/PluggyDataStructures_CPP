# Architecture

LinkedListBrowser separates reusable UI behavior, menu navigation, registration, business operations, data loading, and sorting.

## Runtime Flow

```text
main.cpp
-> app/App.cpp
-> MainMenu
-> DataSourceMenu
-> FileLoader
-> TargetList
-> TargetDataStructureMenu
-> CommandRegistry action
-> optional SortTypeMenu
-> SortRegistry action
-> SortSupport
```

`TargetProgram` owns the active `TargetList`, selected data path, and exit state. It implements list operations but does not register commands or render application menus.

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
- `TargetDataStructureMenu` gets commands from `CommandRegistry`.
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

`Menu` sits below menu controllers as a reusable console helper. Registries do not render menus. Core containers do not depend on registries.

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

`CommandRegistry` and `SortRegistry` derive from `OperationRegistry<TargetProgram>`; `CommandPlugin` and `SortCommand` are aliases of `Operation<TargetProgram>`. `StructureRegistry<SessionT>` is a single template (aliased as `StackRegistry`, `QueueRegistry`, `TreeRegistry`, `GraphRegistry`, `HashTableRegistry`) that seeds its own Exit item.

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
```

Core containers remain domain-neutral; sessions own interactive I/O; operation modules in `src/operations/` self-register. The graph reads an optional `<dataset>.edges` companion file via `FileLoader`.

## Build Note: Self-Registration

Operation, command, and sort modules register through global initializers. The CMake build compiles all `src/` files into an OBJECT library linked directly into both executables, ensuring the linker retains those object files so registration runs before `main`.

## Future Systems

The same spine supports further additions (for example a search system) by adding a `core/` structure, an `XSession`, an `XRegistry` alias, and one `src/operations/` module, then routing it from `StructureMenu`. No changes to `Menu` or the generic spine are required.
