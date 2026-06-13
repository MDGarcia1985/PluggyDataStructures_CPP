# LinkedListBrowser

LinkedListBrowser is a C++17 learning project that loads TXT and CSV datasets into a doubly linked list. Its UI, registries, commands, sorting support, file loading, and core data structures are separated so new menu systems and plugins can be added without rewriting the generic menu layer.

## Build And Run

From the project root:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude main.cpp \
  src/app/*.cpp src/commands/*.cpp src/algorithms/*.cpp \
  src/core/*.cpp src/io/*.cpp src/registry/*.cpp \
  src/sorting/*.cpp src/ui/*.cpp \
  -o LinkedListBrowser
./LinkedListBrowser
```

Build and run tests:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/ApplicationTests.cpp \
  src/app/*.cpp src/commands/*.cpp src/algorithms/*.cpp \
  src/core/*.cpp src/io/*.cpp src/registry/*.cpp \
  src/sorting/*.cpp src/ui/*.cpp \
  -o ApplicationTests
./ApplicationTests
```

On Windows, `.exe` output names may be used. The VS Code task in `.vscode/tasks.json` lists every reorganized source file for `cl.exe`.

## Dataset Support

The startup flow scans `data/` and displays all regular files in filename order.

```text
.txt -> plain text/list loader
.csv -> structured table loader
other -> displayed as unsupported
```

TXT files accept either `fieldOne|fieldTwo` records or ordinary one-item lines. CSV files use the first row as headings, map the first column to `Target::fieldOne()`, and store labeled remaining values in `fieldTwo()`.

## Menu Flow

```text
MainMenu
-> DataSourceMenu
-> TargetDataStructureMenu
-> registered command
-> optional SortTypeMenu
-> registered sorting algorithm
```

`Menu` is generic. It only displays string labels, prompts for numbers, validates ranges, returns selected positions, and prints the shared invalid-selection message.

Menu-specific decisions live in dedicated controllers:

- `MainMenu` starts the application flow.
- `DataSourceMenu` discovers and classifies datasets.
- `TargetDataStructureMenu` executes registered list commands.
- `SortTypeMenu` executes registered sorting algorithms.

## Plugin Registration

Target/list commands are independent source files in `src/commands/`. Each defines a small adapter and registers it with `CommandRegistry`.

```cpp
LLB_REGISTER_COMMAND(9, "New command", llb::newCommand)
```

Exit commands use `LLB_REGISTER_EXIT_COMMAND`. `CommandRegistry` orders regular commands by ID and always moves Exit to the end.

Sorting algorithms register with `SortRegistry`:

```cpp
LLB_REGISTER_SORT("Algorithm Name", llb::algorithmCommand)
```

`SortRegistry` owns its Exit item and returns it last. Adding an algorithm does not require changing `SortTypeMenu`.

## Project Layout

```text
include/
  app/
    App.h
  core/
    Header.h
    Target.h
    TargetList.h
    TargetProgram.h
    TargetQueue.h
    TargetStack.h
  io/
    FileLoader.h
  registry/
    RegistryBase.h
    CommandRegistry.h
    SortRegistry.h
  sorting/
    SortSupport.h
  ui/
    Display.h
    Menu.h
    MainMenu.h
    DataSourceMenu.h
    TargetDataStructureMenu.h
    SortTypeMenu.h

src/
  app/
    App.cpp
  commands/
    AddTargetCommand.cpp
    DeleteTargetCommand.cpp
    DisplayListCommand.cpp
    ExitCommand.cpp
    FindTargetCommand.cpp
    MoveBackwardCommand.cpp
    MoveForwardCommand.cpp
    SortListCommand.cpp
  algorithms/
    InsertionSort.cpp
    SelectionSort.cpp
  core/
    Target.cpp
    TargetList.cpp
    TargetProgram.cpp
    TargetQueue.cpp
    TargetStack.cpp
  io/
    FileLoader.cpp
  registry/
    CommandRegistry.cpp
    SortRegistry.cpp
  sorting/
    SortSupport.cpp
  ui/
    Display.cpp
    Menu.cpp
    MainMenu.cpp
    DataSourceMenu.cpp
    TargetDataStructureMenu.cpp
    SortTypeMenu.cpp
```

## Extension Points

Future systems such as tree, graph, hash table, or search menus can reuse `Menu` and follow the existing controller plus registry pattern. Their navigation and registration policies can remain independent from the target-list and sort registries.

New and modified functions use the standard `Purpose`, `Design`, `Workflow`, and `Data Handoff` header.
