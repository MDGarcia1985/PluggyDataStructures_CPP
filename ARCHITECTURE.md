# LinkedListBrowser Architecture

LinkedListBrowser is organized as a small modular C++ console app. The project keeps `main.cpp` tiny, stores websites in a real doubly linked list, and lets menu commands register themselves through a simple plugin-style registry.

## Project Map

<pre>
LinkedLists/
├── <a href="ARCHITECTURE.md">ARCHITECTURE.md</a>
├── <a href="README.md">README.md</a>
├── <a href="main.cpp">main.cpp</a>
├── data/
│   └── <a href="data/websites.txt">websites.txt</a>
├── include/
│   ├── <a href="include/App.h">App.h</a>
│   ├── <a href="include/Display.h">Display.h</a>
│   ├── <a href="include/FileLoader.h">FileLoader.h</a>
│   ├── <a href="include/Header.h">Header.h</a>
│   ├── <a href="include/Menu.h">Menu.h</a>
│   ├── <a href="include/Website.h">Website.h</a>
│   ├── <a href="include/WebsiteList.h">WebsiteList.h</a>
│   └── <a href="include/WebsiteProgram.h">WebsiteProgram.h</a>
└── src/
    ├── <a href="src/Display.cpp">Display.cpp</a>
    ├── <a href="src/FileLoader.cpp">FileLoader.cpp</a>
    ├── <a href="src/Menu.cpp">Menu.cpp</a>
    ├── <a href="src/Website.cpp">Website.cpp</a>
    ├── <a href="src/WebsiteList.cpp">WebsiteList.cpp</a>
    └── <a href="src/WebsiteProgram.cpp">WebsiteProgram.cpp</a>
</pre>

## Big Picture Flow

```text
┌────────────┐
│ main.cpp   │
└─────┬──────┘
      │ calls llb::runApp()
      ▼
┌──────────────────────┐
│ WebsiteProgram       │
│ app controller       │
└─────┬────────────────┘
      │ loads data
      ▼
┌──────────────────────┐
│ FileLoader           │
│ data/websites.txt    │
└─────┬────────────────┘
      │ fills
      ▼
┌──────────────────────┐
│ WebsiteList          │
│ doubly linked list   │
└─────┬────────────────┘
      │ displayed by
      ▼
┌──────────────────────┐
│ Display              │
│ console output       │
└──────────────────────┘

┌──────────────────────┐
│ Menu                 │
│ reads user choice    │
└─────┬────────────────┘
      │ finds command in
      ▼
┌──────────────────────┐
│ CommandRegistry      │
│ registered actions   │
└─────┬────────────────┘
      │ runs selected action against
      ▼
┌──────────────────────┐
│ WebsiteProgram       │
└──────────────────────┘
```

## Design Goals

1. Keep [`main.cpp`](main.cpp) minimal.
2. Store websites in [`WebsiteList`](include/WebsiteList.h), a real linked list instead of a vector.
3. Separate data, display, file loading, menu input, and controller logic.
4. Support plugin-style menu commands through [`CommandRegistry`](include/Menu.h).
5. Make it easy to add a command without understanding the whole codebase.

## Module Guide

### App Entry

[`main.cpp`](main.cpp) is intentionally small. It includes [`App.h`](include/App.h) and starts the app:

```cpp
#include "include/App.h"

int main()
{
    return llb::runApp();
}
```

[`include/App.h`](include/App.h) is the single top-level include for the application. It gathers the project modules so `main.cpp` and command plugins can include one file.

[`include/Header.h`](include/Header.h) contains shared standard library includes and project-wide constants, including the app name and default data file path.

### Data Model

[`include/Website.h`](include/Website.h) and [`src/Website.cpp`](src/Website.cpp) define one website record. A `Website` stores the site's name and URL, plus website-specific behavior such as case-insensitive matching for search.

[`include/WebsiteList.h`](include/WebsiteList.h) and [`src/WebsiteList.cpp`](src/WebsiteList.cpp) implement the doubly linked list. Each node stores one `Website`.

The list tracks:

- `head_`
- `tail_`
- `current_`
- `count_`

`WebsiteList` owns all node memory. `addBack()` allocates nodes, `removeAt()` deletes one node, and `clear()` deletes the full chain. The destructor calls `clear()`.

The `current_` pointer supports browser-like movement:

- moving forward goes to `current_->next`
- moving backward goes to `current_->previous`
- movement wraps around at the first and last items

### Controller

[`include/WebsiteProgram.h`](include/WebsiteProgram.h) and [`src/WebsiteProgram.cpp`](src/WebsiteProgram.cpp) contain the app controller.

`WebsiteProgram` owns the `WebsiteList`, loads initial data, runs the menu loop, and exposes a small public surface for commands:

```cpp
program.list();
program.displayList();
program.displayCurrent();
program.goForward();
program.goBackward();
program.addWebsiteFromUser();
program.deleteWebsiteFromUser();
program.findWebsiteFromUser();
program.requestExit();
```

The built-in commands are also registered in [`src/WebsiteProgram.cpp`](src/WebsiteProgram.cpp).

### Menu And Commands

[`include/Menu.h`](include/Menu.h) and [`src/Menu.cpp`](src/Menu.cpp) handle menu display, numeric input, and command lookup.

The command system has two parts:

- `Menu` prints command labels and reads choices.
- `CommandRegistry` stores command plugins.

Each command plugin has:

- a numeric command ID
- a menu label
- an action function that receives `WebsiteProgram&`

### File Loading

[`include/FileLoader.h`](include/FileLoader.h) and [`src/FileLoader.cpp`](src/FileLoader.cpp) load starter websites from [`data/websites.txt`](data/websites.txt).

If the data file is missing or empty, `FileLoader` loads the built-in fallback websites.

### Display

[`include/Display.h`](include/Display.h) and [`src/Display.cpp`](src/Display.cpp) centralize console output. This keeps list operations and formatting separate.

## Built-In Commands

The default menu commands are registered in [`src/WebsiteProgram.cpp`](src/WebsiteProgram.cpp):

```text
1. Display the list
2. Go forward and display the webpage
3. Go backward and display the webpage
4. Add another item to the list
5. Delete an item from the list
6. Find an item in the list
7. Exit
```

## Adding A New Command

Add a command in any `.cpp` file that is compiled with the project.

### 1. Include The App Bundle

```cpp
#include "App.h"
```

### 2. Write A Command Function

The function must accept `llb::WebsiteProgram&`.

```cpp
static void countWebsites(llb::WebsiteProgram& program)
{
    llb::Display::printMessage(
        "Total websites: " + std::to_string(program.list().size()));
}
```

### 3. Register The Command

Use `LLB_REGISTER_COMMAND` from [`include/Menu.h`](include/Menu.h):

```cpp
LLB_REGISTER_COMMAND(8, "Count websites", countWebsites)
```

### Complete Example

```cpp
#include "App.h"

static void countWebsites(llb::WebsiteProgram& program)
{
    llb::Display::printMessage(
        "Total websites: " + std::to_string(program.list().size()));
}

LLB_REGISTER_COMMAND(8, "Count websites", countWebsites)
```

The plugin only needs the public app surface. It does not need to know how `WebsiteList`, `Menu`, or `main.cpp` are implemented internally.

## Why A Doubly Linked List?

The assignment requires linked-list behavior, and the menu supports both forward and backward movement. A doubly linked list is the natural fit because each node has both a `next` pointer and a `previous` pointer.

```text
nullptr
   ▲
   │ previous
┌──────┐      next      ┌──────┐      next      ┌──────┐
│ Node │ ─────────────▶ │ Node │ ─────────────▶ │ Node │
└──────┘ ◀───────────── └──────┘ ◀───────────── └──────┘
          previous               previous          │
                                                    ▼
                                                 nullptr
```

The list keeps direct pointers to the first item, last item, and current item so browser-style navigation stays simple.

## Extension Boundaries

New commands should talk to the application through `WebsiteProgram` methods instead of reaching into private list or menu internals.

Good extension points:

- [`WebsiteProgram`](include/WebsiteProgram.h) for app actions
- [`WebsiteList`](include/WebsiteList.h) for list operations
- [`Display`](include/Display.h) for console output
- [`LLB_REGISTER_COMMAND`](include/Menu.h) for menu registration

Files that usually should not need changes for a new command:

- [`main.cpp`](main.cpp)
- [`src/Menu.cpp`](src/Menu.cpp)
- existing data-structure internals in [`src/WebsiteList.cpp`](src/WebsiteList.cpp)

## Test Coverage

[`tests/ApplicationTests.cpp`](tests/ApplicationTests.cpp) is a no-dependency test runner that checks each application level:

- model tests for [`Website`](include/Website.h)
- data-structure tests for [`WebsiteList`](include/WebsiteList.h)
- file-loading tests for [`FileLoader`](include/FileLoader.h)
- output-formatting tests for [`Display`](include/Display.h)
- menu and command-registry tests for [`Menu`](include/Menu.h)
- controller tests for [`WebsiteProgram`](include/WebsiteProgram.h)

The tests use temporary input files and redirected console streams so they can exercise interactive behavior without requiring a person to type menu choices.
