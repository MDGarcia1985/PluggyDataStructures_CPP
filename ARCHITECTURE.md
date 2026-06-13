# Architecture

LinkedListBrowser started as a website-specific linked-list assignment. It is now moving toward a generic Target-based architecture that demonstrates reusable data structures.

The central idea is simple: the linked list stores `Target` records, not websites, messages, tasks, or any other specific domain object. The meaning of each record comes from the selected data file and the user interface text around it.

## Data Flow

```text
data/<selected_file>
-> FileLoader
-> Target
-> TargetList
-> TargetProgram
-> Menu
-> Display
```

`data/<selected_file>` is chosen by the user through `Menu`. Current sample files are `data/websites.txt` and `data/messages.txt`.

`FileLoader` reads the selected file, skips comments and malformed rows, splits valid rows into two pipe-delimited fields, and creates `Target` objects.

`Target` stores two generic fields:

```text
fieldOne
fieldTwo
```

For `websites.txt`, those fields mean website name and URL. For `messages.txt`, those fields mean sender and message text.

`TargetList` stores `Target` objects with linked-list behavior. It owns its nodes, supports add, delete, navigation, search, vector snapshots for display, clearing, copy/move behavior, and destructor cleanup.

`TargetProgram` is the application spine/controller. It coordinates loading data, running menu commands, moving data between modules, and maintaining the current app state.

`Menu` asks the user what to do. It owns input prompts, command display, numeric input validation, and data-source selection.

`Display` prints results. It should format output only and should not own application decisions or linked-list behavior.

## Module Rules

- `TargetList` should not depend on `FileLoader`, `Menu`, `Display`, or `TargetProgram`.
- `TargetList` should store generic `Target` objects and should not assume a record is a website, message, task, or inventory item.
- `FileLoader` may create `Target` objects and load them into `TargetList`.
- `FileLoader` should accept a selected file path such as `data/websites.txt` or `data/messages.txt`.
- `TargetProgram` coordinates the system and is the place where the main workflow comes together.
- `Menu` asks the user what to do, including which data source to load.
- `Display` prints results and status messages.
- `Header.h` is for shared standard library includes, constants, and small common helpers only.
- `Header.h` should not become a dumping ground for class dependencies.
- `App.h` is for top-level application bundling only and should be used by `main.cpp` or the application entry point.

## Key Types

### Target

`Target` is a generic two-field record. It supports reading and updating both fields, case-insensitive matching, and display-string formatting.

### TargetList

`TargetList` is the linked-list container. It manages node memory directly and keeps node pointers private.

Required linked-list operations include:

- add target
- delete target
- display list through a vector snapshot passed to `Display`
- clear list
- destructor cleanup
- current-item navigation
- search by either field

### TargetStack

`TargetStack` demonstrates LIFO behavior with generic targets. `push()` adds a `Target`; `pop()` removes and returns the most recently added `Target`.

### TargetQueue

`TargetQueue` demonstrates FIFO behavior with generic targets. `enqueue()` adds a `Target`; `dequeue()` removes and returns the oldest `Target`.

### FileLoader

`FileLoader` parses simple two-field data files. It does not decide what the fields mean; it only creates `Target` records from the selected file.

### TargetProgram

`TargetProgram` owns the active `TargetList`, tracks whether the user requested exit, asks `Menu` for user choices, calls `FileLoader`, and sends output requests to `Display`.

## Current Layout

```text
include/
  App.h
  Display.h
  FileLoader.h
  Header.h
  Menu.h
  ElectronicsSortSupport.h
  Target.h
  TargetList.h
  TargetProgram.h
  TargetQueue.h
  TargetStack.h

src/
  algorithms/
    InsertionSort.cpp
    SelectionSort.cpp
  ui/
    Display.cpp
    Menu.cpp
  utils/
    ElectronicsSortSupport.cpp
    FileLoader.cpp
  Target.cpp
  TargetList.cpp
  TargetProgram.cpp
  TargetQueue.cpp
  TargetStack.cpp

data/
  websites.txt
  messages.txt

tests/
  ApplicationTests.cpp
```

## Function Notes

Function comments should stay clear enough for a junior C++ student to follow. Continue using these sections:

- Purpose
- Design
- Workflow
- Data Handoff

The notes should explain how data moves between modules without making comments longer than the code they describe.
