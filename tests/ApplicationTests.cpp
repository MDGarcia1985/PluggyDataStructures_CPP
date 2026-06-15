/*
 * File: ApplicationTests.cpp
 * Description: Core model, list, file, display, registry, and controller tests.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "TestHarness.h"

#include "core/TargetProgram.h"
#include "core/TargetQueue.h"
#include "core/TargetStack.h"
#include "io/FileLoader.h"
#include "registry/CommandRegistry.h"
#include "registry/SortRegistry.h"
#include "sorting/SortSupport.h"
#include "ui/DataSourceMenu.h"
#include "ui/Display.h"
#include "ui/Menu.h"

#include <filesystem>


using namespace llbtest;

LLB_TEST(testTargetModel)
{
    llb::Target target("OpenAI", "https://www.openai.com");

    expectEqual(target.fieldOne(), "OpenAI", "Target stores first field.");
    expectEqual(target.fieldTwo(), "https://www.openai.com", "Target stores second field.");
    expectEqual(target.toDisplayString(), "OpenAI - https://www.openai.com", "Target formats display text.");
    expect(target.matches("openai"), "Target matches lowercase first-field search.");
    expect(target.matches("OPENAI.COM"), "Target matches uppercase second-field search.");
    expect(!target.matches("example"), "Target rejects non-matching search.");

    target.setFieldOne("Example");
    target.setFieldTwo("https://example.com");

    expectEqual(target.fieldOne(), "Example", "Target updates first field.");
    expectEqual(target.fieldTwo(), "https://example.com", "Target updates second field.");
}

LLB_TEST(testTargetListDataStructure)
{
    llb::TargetList targets;

    expect(targets.isEmpty(), "New list starts empty.");
    expectEqual(targets.size(), 0, "New list size is zero.");
    expect(targets.current() == nullptr, "New list has no current target.");
    expect(!targets.moveForward(), "Empty list cannot move forward.");
    expect(!targets.moveBackward(), "Empty list cannot move backward.");

    targets.addBack(llb::Target("Alpha", "https://alpha.test"));
    targets.addBack(llb::Target("Beta", "https://beta.test"));
    targets.addBack(llb::Target("Gamma", "https://gamma.test"));

    expectEqual(targets.size(), 3, "List tracks added targets.");
    expectEqual(targets.current()->fieldOne(), "Alpha", "First added target becomes current.");

    expect(targets.moveForward(), "List moves forward.");
    expectEqual(targets.current()->fieldOne(), "Beta", "Forward movement selects second target.");
    expect(targets.moveForward(), "List moves forward again.");
    expectEqual(targets.current()->fieldOne(), "Gamma", "Forward movement selects third target.");
    expect(targets.moveForward(), "Forward movement wraps.");
    expectEqual(targets.current()->fieldOne(), "Alpha", "Forward wrap selects first target.");

    expect(targets.moveBackward(), "List moves backward.");
    expectEqual(targets.current()->fieldOne(), "Gamma", "Backward movement wraps to tail.");

    expectEqual(targets.front()->fieldOne(), "Alpha", "Front reads the head target.");
    expectEqual(targets.back()->fieldOne(), "Gamma", "Back reads the tail target.");

    expect(targets.setCurrentToPosition(2), "List sets current by position.");
    expectEqual(targets.current()->fieldOne(), "Beta", "Position selection chooses expected target.");
    expectEqual(targets.currentPosition(), 2, "List reports current position.");

    std::vector<llb::TargetSearchResult> results = targets.findAll("TA");
    expectEqual(results.size(), 1, "Search finds case-insensitive result.");
    expectEqual(results.front().position, 2, "Search reports one-based position.");
    expectEqual(results.front().target.fieldOne(), "Beta", "Search returns matched target.");

    expect(targets.removeAt(2), "List removes selected position.");
    expectEqual(targets.size(), 2, "List size decreases after remove.");
    expectEqual(targets.current()->fieldOne(), "Gamma", "Removing current advances to next target.");
    expect(!targets.removeAt(42), "List rejects invalid remove position.");

    const std::vector<llb::Target> values = targets.toVector();
    expectEqual(values.size(), 2, "List converts to vector.");
    expectEqual(values[0].fieldOne(), "Alpha", "Vector keeps first remaining target.");
    expectEqual(values[1].fieldOne(), "Gamma", "Vector keeps second remaining target.");

    llb::TargetList copied(targets);
    expectEqual(copied.size(), targets.size(), "Copy constructor copies size.");
    expectEqual(copied.current()->fieldOne(), targets.current()->fieldOne(),
        "Copy constructor preserves current target.");

    llb::TargetList assigned;
    assigned = targets;
    expectEqual(assigned.size(), targets.size(), "Copy assignment copies size.");
    expectEqual(assigned.current()->fieldOne(), targets.current()->fieldOne(),
        "Copy assignment preserves current target.");

    llb::TargetList moved(std::move(copied));
    expectEqual(moved.size(), 2, "Move constructor transfers nodes.");
    expect(copied.isEmpty(), "Move constructor leaves source empty.");
}

LLB_TEST(testListEndpointRemoval)
{
    llb::TargetList targets;
    targets.addBack(llb::Target("One", "1"));
    targets.addBack(llb::Target("Two", "2"));
    targets.addBack(llb::Target("Three", "3"));

    expect(targets.removeBack(), "removeBack removes the tail.");
    expectEqual(targets.size(), 2, "removeBack decreases size.");
    expectEqual(targets.back()->fieldOne(), "Two", "removeBack exposes the new tail.");

    expect(targets.removeFront(), "removeFront removes the head.");
    expectEqual(targets.size(), 1, "removeFront decreases size.");
    expectEqual(targets.front()->fieldOne(), "Two", "removeFront exposes the new head.");

    expect(targets.removeBack(), "removeBack removes the last element.");
    expect(targets.isEmpty(), "List is empty after removing all elements.");
    expect(!targets.removeFront(), "removeFront fails on an empty list.");
    expect(!targets.removeBack(), "removeBack fails on an empty list.");
    expect(targets.front() == nullptr, "front is null on an empty list.");
    expect(targets.back() == nullptr, "back is null on an empty list.");
}

LLB_TEST(testStackAndQueue)
{
    llb::TargetStack stack;
    stack.push(llb::Target("First", "1"));
    stack.push(llb::Target("Second", "2"));

    expectEqual(stack.size(), 2, "Stack tracks pushed targets.");
    expectEqual(stack.peek().fieldOne(), "Second", "Stack peek shows the most recent target.");
    expectEqual(stack.pop().fieldOne(), "Second", "Stack pops most recently pushed target.");
    expectEqual(stack.pop().fieldOne(), "First", "Stack pops older target second.");
    expect(stack.isEmpty(), "Stack reports empty after pops.");

    llb::TargetQueue queue;
    queue.enqueue(llb::Target("First", "1"));
    queue.enqueue(llb::Target("Second", "2"));

    expectEqual(queue.size(), 2, "Queue tracks enqueued targets.");
    expectEqual(queue.peek().fieldOne(), "First", "Queue peek shows the oldest target.");
    expectEqual(queue.dequeue().fieldOne(), "First", "Queue dequeues oldest target first.");
    expectEqual(queue.dequeue().fieldOne(), "Second", "Queue dequeues newer target second.");
    expect(queue.isEmpty(), "Queue reports empty after dequeues.");
}

LLB_TEST(testFileLoadingLayer)
{
    const std::string path = "tests/tmp_targets_test.txt";

    writeFile(path,
        "# comment line\n"
        "  Alpha Site  |  https://alpha.test  \n"
        "missing separator\n"
        "No Url |   \n"
        "Beta Site|https://beta.test\n");

    llb::TargetList loaded;
    expect(llb::FileLoader::loadTargetsFromFile(path, loaded), "File loader loads valid rows.");
    expectEqual(loaded.size(), 4, "Text loader accepts pipe records and ordinary list items.");
    expectEqual(loaded.toVector()[0].fieldOne(), "Alpha Site", "File loader trims first fields.");
    expectEqual(loaded.toVector()[0].fieldTwo(), "https://alpha.test", "File loader trims second fields.");
    expectEqual(loaded.toVector()[1].fieldOne(), "missing separator", "Text loader keeps plain list lines.");
    expectEqual(loaded.toVector()[2].fieldOne(), "No Url", "Text loader accepts an empty optional second field.");
    expectEqual(loaded.toVector()[3].fieldOne(), "Beta Site", "File loader loads later valid rows.");

    llb::TargetList missing;
    expect(!llb::FileLoader::loadTargetsFromFile("tests/does_not_exist.txt", missing),
        "File loader reports missing file.");
    expect(missing.isEmpty(), "Missing file leaves list empty.");

    writeFile(path, "# only comments\n\n");

    llb::TargetList empty;
    expect(!llb::FileLoader::loadTargetsFromFile(path, empty), "File loader reports no loaded targets.");
    expect(empty.isEmpty(), "Empty data file leaves list empty.");

    llb::FileLoader::loadFallbackTargets(empty);
    expectEqual(empty.size(), 20, "Fallback loader adds built-in targets.");
    expectEqual(empty.toVector().front().fieldOne(), "Google", "Fallback loader starts with Google.");

    const std::string csvPath = "tests/tmp_targets_test.csv";
    writeFile(csvPath,
        "name,category,notes\n"
        "Alpha,First,\"contains, comma\"\n"
        "Beta,Second,plain\n");

    llb::TargetList csvTargets;
    expect(llb::FileLoader::loadTargetsFromFile(csvPath, csvTargets), "CSV loader loads table rows.");
    expectEqual(csvTargets.size(), 2, "CSV loader excludes the heading row.");
    expectEqual(csvTargets.toVector()[0].fieldOne(), "Alpha", "CSV loader uses the first column as field one.");
    expectEqual(csvTargets.toVector()[0].fieldTwo(), "category=First; notes=contains, comma",
        "CSV loader labels remaining columns and handles quoted commas.");
    expect(llb::FileLoader::fileType(csvPath) == llb::DataFileType::Csv, "CSV extension is supported.");
    expect(llb::FileLoader::fileType("data/example.json") == llb::DataFileType::Unsupported,
        "Unknown extensions are unsupported.");
    expect(!llb::FileLoader::discoverDataFiles().empty(), "Data files are discovered dynamically.");

    std::remove(path.c_str());
    std::remove(csvPath.c_str());
}

LLB_TEST(testEdgeFileLoadingLayer)
{
    const std::string nodePath = "tests/tmp_graph_nodes.csv";
    const std::string edgePath = "tests/tmp_graph_nodes.edges";

    writeFile(nodePath, "name,role\nAlice,Engineer\nBob,Designer\n");
    writeFile(edgePath,
        "# from|to|weight\n"
        "Alice|Bob|2.5\n"
        "Bob|Alice\n");

    expectEqual(llb::FileLoader::discoverEdgeFile(nodePath), edgePath,
        "Edge discovery finds the companion .edges file.");

    const std::vector<llb::EdgeRecord> edges = llb::FileLoader::loadEdges(edgePath);
    expectEqual(edges.size(), 2, "Edge loader skips comments and reads edge rows.");
    expectEqual(edges[0].from, "Alice", "Edge loader reads the source key.");
    expectEqual(edges[0].to, "Bob", "Edge loader reads the destination key.");
    expect(edges[0].weight > 2.49 && edges[0].weight < 2.51, "Edge loader parses the weight.");
    expect(edges[1].weight > 0.99 && edges[1].weight < 1.01, "Edge loader defaults a missing weight to 1.");

    expect(llb::FileLoader::discoverEdgeFile("tests/no_such_nodes.csv").empty(),
        "Edge discovery returns empty when no companion file exists.");

    std::remove(nodePath.c_str());
    std::remove(edgePath.c_str());
}

LLB_TEST(testDisplayLayer)
{
    {
        ScopedCoutCapture output;
        llb::Display::printTarget(llb::Target("Alpha", "https://alpha.test"), 3);
        expect(contains(output.text(), "3) Alpha - https://alpha.test"), "Display prints numbered target.");
    }

    {
        ScopedCoutCapture output;
        llb::Display::printTargetList(std::vector<llb::Target>());
        expect(contains(output.text(), "The target list is empty."), "Display prints empty list message.");
    }

    {
        std::vector<llb::Target> targets;
        targets.push_back(llb::Target("Alpha", "https://alpha.test"));
        targets.push_back(llb::Target("Beta", "https://beta.test"));

        ScopedCoutCapture output;
        llb::Display::printTargetList(targets);
        expect(contains(output.text(), "Target List:"), "Display prints list heading.");
        expect(contains(output.text(), "1) Alpha - https://alpha.test"), "Display prints first list item.");
        expect(contains(output.text(), "2) Beta - https://beta.test"), "Display prints second list item.");
    }

    {
        std::vector<llb::Target> targets;
        targets.push_back(llb::Target("Alpha", "1"));

        ScopedCoutCapture output;
        llb::Display::printTargets("In-order traversal", targets);
        expect(contains(output.text(), "In-order traversal:"), "Display prints a custom heading.");
        expect(contains(output.text(), "1) Alpha - 1"), "Display numbers labeled targets.");
    }

    {
        ScopedCoutCapture output;
        llb::Display::printCurrentTarget(nullptr, 0);
        expect(contains(output.text(), "No current record to display."), "Display handles null current target.");
    }
}

LLB_TEST(testMenuAndCommandLayer)
{
    bool commandRan = false;
    llb::CommandPlugin command{98, "Test command", [&commandRan](llb::TargetProgram&)
        {
            commandRan = true;
        }};

    expect(llb::CommandRegistry::instance().registerCommand(command), "Command registry accepts valid command.");
    expect(!llb::CommandRegistry::instance().registerCommand(command), "Command registry rejects duplicate ID.");
    expect(!llb::CommandRegistry::instance().registerCommand(llb::CommandPlugin{0, "Bad", command.action}),
        "Command registry rejects invalid ID.");
    expect(!llb::CommandRegistry::instance().registerCommand(llb::CommandPlugin{99, "", command.action}),
        "Command registry rejects empty label.");

    llb::TargetProgram program;
    const llb::CommandPlugin* found = llb::CommandRegistry::instance().findById(98);
    expect(found != nullptr, "Command registry finds registered command.");
    found->action(program);
    expect(commandRan, "Command registry stores runnable action.");

    const std::vector<llb::CommandPlugin> commands = llb::CommandRegistry::instance().commands();
    expect(commands.back().isExit, "Exit is always the final generated main-menu command.");
    for (std::size_t i = 1; i + 1 < commands.size(); ++i)
    {
        expect(commands[i - 1].id < commands[i].id, "Command registry returns commands sorted by ID.");
    }

    {
        const std::vector<std::string> menuOptions = {"Second", "First"};

        ScopedCoutCapture output;
        llb::Menu::display("Test Menu", menuOptions);
        expect(contains(output.text(), "Test Menu"), "Menu displays the supplied title.");
        expect(contains(output.text(), "1) Second"), "Menu numbers commands by generated position.");
        expect(contains(output.text(), "2) First"), "Menu displays every command label.");
    }

    {
        const std::vector<std::string> files = llb::FileLoader::discoverDataFiles();
        std::size_t messagesPosition = 0;
        for (std::size_t index = 0; index < files.size(); ++index)
        {
            if (std::filesystem::path(files[index]).filename() == "messages.txt")
            {
                messagesPosition = index + 1;
            }
        }

        ScopedCinInput input(std::to_string(messagesPosition) + "\n");
        ScopedCoutCapture output;
        expectEqual(llb::DataSourceMenu::selectDataSource(), "data/messages.txt",
            "Data source menu selects messages file.");
    }

    {
        ScopedCinInput input("0\n999\n2\n");
        ScopedCoutCapture output;
        expectEqual(llb::Menu::promptSelection(3), 1, "Menu selection retries until the range is valid.");
        expect(contains(output.text(), "Invalid selection."), "Menu reports out-of-range selections.");
    }

    {
        ScopedCinInput input("abc\n42\n");
        ScopedCoutCapture output;
        const int choice = llb::Menu::promptInteger("Number: ");
        expect(choice == 42, "Menu keeps prompting until numeric input is provided.");
        expect(contains(output.text(), "Invalid input."), "Menu reports invalid numeric input.");
    }

    const std::vector<llb::SortCommand> sortCommands = llb::SortRegistry::instance().commands();
    expect(sortCommands.size() >= 3,
        "Sort modules register themselves with the Sort Type Menu.");
    expect(sortCommands.back().isExit, "Sort registry keeps Exit as the final option.");
    expect(llb::targetLess(llb::Target("alpha", "2"), llb::Target("Beta", "1")),
        "Generic sort comparison orders targets without dataset assumptions.");
}

LLB_TEST(testControllerLayer)
{
    const std::string path = "tests/tmp_program_data.txt";
    writeFile(path, "Alpha|https://alpha.test\nBeta|https://beta.test\nGamma|https://gamma.test\n");

    llb::TargetProgram program(path);
    program.loadInitialData();

    expectEqual(program.list().size(), 3, "Program loads initial data into its list.");
    expectEqual(program.list().current()->fieldOne(), "Alpha", "Program starts at first loaded target.");

    {
        ScopedCoutCapture output;
        program.goForward();
        expectEqual(program.list().current()->fieldOne(), "Beta", "Program moves forward through list.");
        expect(contains(output.text(), "Current record [2]"), "Program displays current target after forward move.");
    }

    {
        ScopedCoutCapture output;
        program.goBackward();
        expectEqual(program.list().current()->fieldOne(), "Alpha", "Program moves backward through list.");
        expect(contains(output.text(), "Current record [1]"), "Program displays current target after backward move.");
    }

    {
        ScopedCinInput input("Gamma\n");
        ScopedCoutCapture output;
        program.findTargetFromUser();
        expectEqual(program.list().current()->fieldOne(), "Gamma", "Program search sets current to first match.");
        expect(contains(output.text(), "Search Results:"), "Program displays search results.");
    }

    {
        ScopedCinInput input("Delta\nhttps://delta.test\n");
        ScopedCoutCapture output;
        program.addTargetFromUser();
        expectEqual(program.list().size(), 4, "Program adds target from user input.");
        expect(contains(output.text(), "Target added successfully."), "Program reports successful add.");
    }

    {
        ScopedCinInput input("4\n");
        ScopedCoutCapture output;
        program.deleteTargetFromUser();
        expectEqual(program.list().size(), 3, "Program deletes target from user input.");
        expect(contains(output.text(), "Target deleted successfully."), "Program reports successful delete.");
    }

    expect(!program.exitRequested(), "Program starts without exit requested.");
    program.requestExit();
    expect(program.exitRequested(), "Program records exit request.");

    std::remove(path.c_str());
}
