# LinkedListBrowser

LinkedListBrowser began as a website linked-list assignment. It originally loaded website names and URLs into a linked list and let the user browse, add, delete, search, and display those entries.

The project is now being refactored into a generic data-structure framework. The goal is to show that the linked list does not care what the record means. A record can represent a website, a text message, a task, an inventory item, or another simple two-field data type.

## Current Direction

`Target` is the generic record type. It stores:

```text
fieldOne
fieldTwo
```

For `data/websites.txt`, `fieldOne` is the website name and `fieldTwo` is the URL.

For `data/messages.txt`, `fieldOne` is the sender and `fieldTwo` is the message text.

`TargetList` is the linked-list container. It owns the nodes, stores `Target` objects, supports navigation and search, and cleans up node memory in `clear()` and the destructor.

`TargetStack` and `TargetQueue` demonstrate how the same generic target records can also be used for LIFO and FIFO behavior.

## Data Flow

```text
data/<selected_file>
-> FileLoader
-> Target objects
-> TargetList
-> TargetProgram
-> Menu
-> Display
```

`Menu` lets the user select a data source. `FileLoader` reads the selected file from `data/`, parses pipe-delimited rows, and creates `Target` objects. `TargetProgram` coordinates the application, while `Display` handles output formatting only.

## Project Layout

```text
include/
  App.h
  Display.h
  FileLoader.h
  Header.h
  Menu.h
  Target.h
  TargetList.h
  TargetProgram.h
  TargetQueue.h
  TargetStack.h

src/
  Display.cpp
  FileLoader.cpp
  Menu.cpp
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

main.cpp
README.md
ARCHITECTURE.md
```

## Sample Data Format

Each non-comment line uses two fields separated by a pipe:

```text
First Field|Second Field
```

Examples:

```text
OpenAI|https://www.openai.com
Maya|Can you review the linked-list code?
```

Blank lines, comment lines beginning with `#`, malformed rows, and rows with an empty field are skipped.

## Build And Test

Example test build:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/ApplicationTests.cpp src/Target.cpp src/TargetList.cpp src/TargetStack.cpp src/TargetQueue.cpp src/Menu.cpp src/FileLoader.cpp src/Display.cpp src/TargetProgram.cpp -o ApplicationTests
./ApplicationTests
```

Example app build:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude main.cpp src/*.cpp -o LinkedListBrowser
./LinkedListBrowser
```

On Windows with MinGW, use `.exe` output names if preferred.

## Learning Purpose

This project is intentionally written like a real modular C++ repo for learning purposes. The boundaries are deliberately separated:

- `Target` models one generic record.
- `TargetList` owns linked-list behavior.
- `FileLoader` creates targets from selected files.
- `TargetProgram` coordinates the application.
- `Menu` collects user choices.
- `Display` prints formatted output.
- `Header.h` holds common includes, constants, and helpers only.
- `App.h` is the top-level launch bundle for the entry point.

Function header notes should continue to explain:

- Purpose - why the function exists.
- Design - the reasoning behind the implementation.
- Workflow - the sequence of operations performed.
- Data Handoff - how data enters, leaves, or moves to another module.
