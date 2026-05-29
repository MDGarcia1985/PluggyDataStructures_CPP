# LinkedListBrowser

LinkedListBrowser is a C++ console program that stores website entries in a doubly linked list and lets the user move forward, move backward, add, delete, find, and display websites.

The project is intentionally organized like a small repo instead of a single-file assignment. `main.cpp` stays minimal, and the rest of the program is split into focused modules.

## Project layout

```text
LinkedListBrowser/
├── main.cpp
├── README.md
├── ARCHITECTURE.md
├── include/
│   ├── App.h
│   ├── Header.h
│   ├── Website.h
│   ├── WebsiteList.h
│   ├── WebsiteProgram.h
│   ├── Menu.h
│   ├── FileLoader.h
│   └── Display.h
├── src/
│   ├── Website.cpp
│   ├── WebsiteList.cpp
│   ├── WebsiteProgram.cpp
│   ├── Menu.cpp
│   ├── FileLoader.cpp
│   └── Display.cpp
├── data/
│   └── websites.txt
└── tests/
    └── ApplicationTests.cpp
```

## Build and run

From the root directory:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude main.cpp src/*.cpp -o LinkedListBrowser
./LinkedListBrowser
```


On Windows PowerShell with MinGW:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude main.cpp src/*.cpp -o LinkedListBrowser.exe
.\LinkedListBrowser.exe
```


## Run tests

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/ApplicationTests.cpp src/Website.cpp src/WebsiteList.cpp src/Menu.cpp src/FileLoader.cpp src/Display.cpp src/WebsiteProgram.cpp -o ApplicationTests
./ApplicationTests
```


On Windows PowerShell with MinGW:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/ApplicationTests.cpp src/Website.cpp src/WebsiteList.cpp src/Menu.cpp src/FileLoader.cpp src/Display.cpp src/WebsiteProgram.cpp -o ApplicationTests.exe
.\ApplicationTests.exe
```


## Menu options

```text
1) Display the list
2) Go forward and display the webpage
3) Go backward and display the webpage
4) Add another item to the list
5) Delete an item from the list
6) Find an item in the list
7) Exit
```

## Data file format

The starter websites are loaded from `data/websites.txt`.

Each line uses this format:

```text
Website Name|Website URL
```

Lines starting with `#` are ignored.

## Plugin-style command expansion

The command menu is built from registered command plugins instead of hard-coded menu branches inside `main.cpp`. A new command can be added from a new `.cpp` file without editing `main.cpp`, `Menu.cpp`, or the linked-list classes.

Example plugin file:

```cpp
#include "App.h"

static void countWebsites(llb::WebsiteProgram& program)
{
    llb::Display::printMessage(
        "Total websites: " + std::to_string(program.list().size()));
}

LLB_REGISTER_COMMAND(8, "Count websites", countWebsites)
```

Then compile the new plugin file with the rest of the project:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude main.cpp src/*.cpp src/CountWebsitesPlugin.cpp -o LinkedListBrowser
```

The core program does not need to know the plugin exists. The plugin registers itself through `LLB_REGISTER_COMMAND`.

## Algorithm and structure sources

Several algorithm and code-structure ideas came from my own C++ text game work, including menu-building patterns such as a `std::vector<std::string>` based `displayMainMenu` function.

## Funtion Header Notes

Funcitons include comment headers that explain:
- Purpose -– Why the function exists.
- Design -– The reasoning behind the implementation and any important architectural decisions.
- Workflow -– The sequence of operations performed by the function.
- Data Handoff -– The inputs, outputs, and how data is passed to other parts of the system.

This commenting standard was developed over the past three years to improve long-term maintainability and knowledge transfer. It helps me quickly reestablish context when returning to a codebase after an extended period and provides junior developers with a clear understanding of why a function exists, how it operates, and how it fits into the broader system architecture.

## License

This project is licensed under the MIT License. See `LICENSE` for details.