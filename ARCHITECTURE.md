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

## Future Systems

New systems can add their own menu controller and registry while reusing `Menu`:

```text
HashTableMenu + HashTableRegistry
TreeMenu + TreeRegistry
GraphMenu + GraphRegistry
SearchMenu + SearchRegistry
```

This avoids coupling future navigation to the target-list or sorting command sets.
