#include "App.h"

#include <cstdio>
#include <sstream>
#include <stdexcept>

namespace
{
    int testsRun = 0;

    void expect(bool condition, const std::string& message)
    {
        ++testsRun;

        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    void expectEqual(const std::string& actual, const std::string& expected, const std::string& message)
    {
        expect(actual == expected, message + " Expected: [" + expected + "], actual: [" + actual + "]");
    }

    void expectEqual(std::size_t actual, std::size_t expected, const std::string& message)
    {
        expect(actual == expected,
            message + " Expected: " + std::to_string(expected) + ", actual: " + std::to_string(actual));
    }

    class ScopedCoutCapture
    {
    private:
        std::ostringstream captured_;
        std::streambuf* original_;

    public:
        ScopedCoutCapture()
            : captured_(), original_(std::cout.rdbuf(captured_.rdbuf()))
        {
        }

        ~ScopedCoutCapture()
        {
            std::cout.rdbuf(original_);
        }

        std::string text() const
        {
            return captured_.str();
        }
    };

    class ScopedCinInput
    {
    private:
        std::istringstream input_;
        std::streambuf* original_;

    public:
        explicit ScopedCinInput(const std::string& input)
            : input_(input), original_(std::cin.rdbuf(input_.rdbuf()))
        {
        }

        ~ScopedCinInput()
        {
            std::cin.rdbuf(original_);
        }
    };

    bool contains(const std::string& text, const std::string& searchTerm)
    {
        return text.find(searchTerm) != std::string::npos;
    }

    void writeFile(const std::string& path, const std::string& contents)
    {
        std::ofstream output(path);
        output << contents;
    }

    void testWebsiteModel()
    {
        llb::Website website("OpenAI", "https://www.openai.com");

        expectEqual(website.name(), "OpenAI", "Website stores name.");
        expectEqual(website.url(), "https://www.openai.com", "Website stores URL.");
        expectEqual(website.toDisplayString(), "OpenAI - https://www.openai.com", "Website formats display text.");
        expect(website.matches("openai"), "Website matches lowercase name search.");
        expect(website.matches("OPENAI.COM"), "Website matches uppercase URL search.");
        expect(!website.matches("example"), "Website rejects non-matching search.");

        website.setName("Example");
        website.setUrl("https://example.com");

        expectEqual(website.name(), "Example", "Website updates name.");
        expectEqual(website.url(), "https://example.com", "Website updates URL.");
    }

    void testWebsiteListDataStructure()
    {
        llb::WebsiteList websites;

        expect(websites.isEmpty(), "New list starts empty.");
        expectEqual(websites.size(), 0, "New list size is zero.");
        expect(websites.current() == nullptr, "New list has no current website.");
        expect(!websites.moveForward(), "Empty list cannot move forward.");
        expect(!websites.moveBackward(), "Empty list cannot move backward.");

        websites.addBack(llb::Website("Alpha", "https://alpha.test"));
        websites.addBack(llb::Website("Beta", "https://beta.test"));
        websites.addBack(llb::Website("Gamma", "https://gamma.test"));

        expectEqual(websites.size(), 3, "List tracks added websites.");
        expectEqual(websites.current()->name(), "Alpha", "First added website becomes current.");

        expect(websites.moveForward(), "List moves forward.");
        expectEqual(websites.current()->name(), "Beta", "Forward movement selects second website.");
        expect(websites.moveForward(), "List moves forward again.");
        expectEqual(websites.current()->name(), "Gamma", "Forward movement selects third website.");
        expect(websites.moveForward(), "Forward movement wraps.");
        expectEqual(websites.current()->name(), "Alpha", "Forward wrap selects first website.");

        expect(websites.moveBackward(), "List moves backward.");
        expectEqual(websites.current()->name(), "Gamma", "Backward movement wraps to tail.");

        expect(websites.setCurrentToPosition(2), "List sets current by position.");
        expectEqual(websites.current()->name(), "Beta", "Position selection chooses expected website.");
        expectEqual(websites.currentPosition(), 2, "List reports current position.");

        std::vector<llb::WebsiteSearchResult> results = websites.findAll("TA");
        expectEqual(results.size(), 1, "Search finds case-insensitive result.");
        expectEqual(results.front().position, 2, "Search reports one-based position.");
        expectEqual(results.front().website.name(), "Beta", "Search returns matched website.");

        expect(websites.removeAt(2), "List removes selected position.");
        expectEqual(websites.size(), 2, "List size decreases after remove.");
        expectEqual(websites.current()->name(), "Gamma", "Removing current advances to next website.");
        expect(!websites.removeAt(42), "List rejects invalid remove position.");

        const std::vector<llb::Website> values = websites.toVector();
        expectEqual(values.size(), 2, "List converts to vector.");
        expectEqual(values[0].name(), "Alpha", "Vector keeps first remaining website.");
        expectEqual(values[1].name(), "Gamma", "Vector keeps second remaining website.");

        llb::WebsiteList copied(websites);
        expectEqual(copied.size(), websites.size(), "Copy constructor copies size.");
        expectEqual(copied.current()->name(), websites.current()->name(), "Copy constructor preserves current website.");

        llb::WebsiteList assigned;
        assigned = websites;
        expectEqual(assigned.size(), websites.size(), "Copy assignment copies size.");
        expectEqual(assigned.current()->name(), websites.current()->name(), "Copy assignment preserves current website.");

        llb::WebsiteList moved(std::move(copied));
        expectEqual(moved.size(), 2, "Move constructor transfers nodes.");
        expect(copied.isEmpty(), "Move constructor leaves source empty.");
    }

    void testFileLoadingLayer()
    {
        const std::string path = "tests/tmp_websites_test.txt";

        writeFile(path,
            "# comment line\n"
            "  Alpha Site  |  https://alpha.test  \n"
            "missing separator\n"
            "No Url |   \n"
            "Beta Site|https://beta.test\n");

        llb::WebsiteList loaded;
        expect(llb::FileLoader::loadWebsitesFromFile(path, loaded), "File loader loads valid rows.");
        expectEqual(loaded.size(), 2, "File loader skips comments, malformed rows, and incomplete rows.");
        expectEqual(loaded.toVector()[0].name(), "Alpha Site", "File loader trims website names.");
        expectEqual(loaded.toVector()[0].url(), "https://alpha.test", "File loader trims website URLs.");
        expectEqual(loaded.toVector()[1].name(), "Beta Site", "File loader loads later valid rows.");

        llb::WebsiteList missing;
        expect(!llb::FileLoader::loadWebsitesFromFile("tests/does_not_exist.txt", missing),
            "File loader reports missing file.");
        expect(missing.isEmpty(), "Missing file leaves list empty.");

        writeFile(path, "# only comments\n\ninvalid\n");

        llb::WebsiteList empty;
        expect(!llb::FileLoader::loadWebsitesFromFile(path, empty), "File loader reports no loaded websites.");
        expect(empty.isEmpty(), "Empty data file leaves list empty.");

        llb::FileLoader::loadFallbackWebsites(empty);
        expectEqual(empty.size(), 20, "Fallback loader adds built-in websites.");
        expectEqual(empty.toVector().front().name(), "Google", "Fallback loader starts with Google.");

        std::remove(path.c_str());
    }

    void testDisplayLayer()
    {
        {
            ScopedCoutCapture output;
            llb::Display::printWebsite(llb::Website("Alpha", "https://alpha.test"), 3);
            expect(contains(output.text(), "3) Alpha - https://alpha.test"), "Display prints numbered website.");
        }

        {
            ScopedCoutCapture output;
            llb::Display::printWebsiteList(std::vector<llb::Website>());
            expect(contains(output.text(), "The website list is empty."), "Display prints empty list message.");
        }

        {
            std::vector<llb::Website> websites;
            websites.push_back(llb::Website("Alpha", "https://alpha.test"));
            websites.push_back(llb::Website("Beta", "https://beta.test"));

            ScopedCoutCapture output;
            llb::Display::printWebsiteList(websites);
            expect(contains(output.text(), "Website List:"), "Display prints list heading.");
            expect(contains(output.text(), "1) Alpha - https://alpha.test"), "Display prints first list item.");
            expect(contains(output.text(), "2) Beta - https://beta.test"), "Display prints second list item.");
        }

        {
            ScopedCoutCapture output;
            llb::Display::printCurrentWebsite(nullptr, 0);
            expect(contains(output.text(), "No current webpage to display."), "Display handles null current website.");
        }
    }

    void testMenuAndCommandLayer()
    {
        bool commandRan = false;
        llb::CommandPlugin command{98, "Test command", [&commandRan](llb::WebsiteProgram&)
            {
                commandRan = true;
            }};

        expect(llb::CommandRegistry::instance().registerCommand(command), "Command registry accepts valid command.");
        expect(!llb::CommandRegistry::instance().registerCommand(command), "Command registry rejects duplicate ID.");
        expect(!llb::CommandRegistry::instance().registerCommand(llb::CommandPlugin{0, "Bad", command.action}),
            "Command registry rejects invalid ID.");
        expect(!llb::CommandRegistry::instance().registerCommand(llb::CommandPlugin{99, "", command.action}),
            "Command registry rejects empty label.");

        llb::WebsiteProgram program;
        const llb::CommandPlugin* found = llb::CommandRegistry::instance().findById(98);
        expect(found != nullptr, "Command registry finds registered command.");
        found->action(program);
        expect(commandRan, "Command registry stores runnable action.");

        const std::vector<llb::CommandPlugin> commands = llb::CommandRegistry::instance().commands();
        for (std::size_t i = 1; i < commands.size(); ++i)
        {
            expect(commands[i - 1].id < commands[i].id, "Command registry returns commands sorted by ID.");
        }

        {
            std::vector<llb::CommandPlugin> menuCommands;
            menuCommands.push_back(llb::CommandPlugin{2, "Second", [](llb::WebsiteProgram&) {}});
            menuCommands.push_back(llb::CommandPlugin{1, "First", [](llb::WebsiteProgram&) {}});

            ScopedCoutCapture output;
            llb::Menu::display(menuCommands);
            expect(contains(output.text(), "2) Second"), "Menu displays command labels.");
            expect(contains(output.text(), "1) First"), "Menu displays all command labels.");
        }

        {
            ScopedCinInput input("abc\n42\n");
            ScopedCoutCapture output;
            const int choice = llb::Menu::promptInteger("Number: ");
            expect(choice == 42, "Menu keeps prompting until numeric input is provided.");
            expect(contains(output.text(), "Invalid input."), "Menu reports invalid numeric input.");
        }
    }

    void testControllerLayer()
    {
        const std::string path = "tests/tmp_program_data.txt";
        writeFile(path, "Alpha|https://alpha.test\nBeta|https://beta.test\nGamma|https://gamma.test\n");

        llb::WebsiteProgram program(path);
        program.loadInitialData();

        expectEqual(program.list().size(), 3, "Program loads initial data into its list.");
        expectEqual(program.list().current()->name(), "Alpha", "Program starts at first loaded website.");

        {
            ScopedCoutCapture output;
            program.goForward();
            expectEqual(program.list().current()->name(), "Beta", "Program moves forward through list.");
            expect(contains(output.text(), "Current Webpage [2]"), "Program displays current website after forward move.");
        }

        {
            ScopedCoutCapture output;
            program.goBackward();
            expectEqual(program.list().current()->name(), "Alpha", "Program moves backward through list.");
            expect(contains(output.text(), "Current Webpage [1]"), "Program displays current website after backward move.");
        }

        {
            ScopedCinInput input("Gamma\n");
            ScopedCoutCapture output;
            program.findWebsiteFromUser();
            expectEqual(program.list().current()->name(), "Gamma", "Program search sets current to first match.");
            expect(contains(output.text(), "Search Results:"), "Program displays search results.");
        }

        {
            ScopedCinInput input("Delta\nhttps://delta.test\n");
            ScopedCoutCapture output;
            program.addWebsiteFromUser();
            expectEqual(program.list().size(), 4, "Program adds website from user input.");
            expect(contains(output.text(), "Website added successfully."), "Program reports successful add.");
        }

        {
            ScopedCinInput input("4\n");
            ScopedCoutCapture output;
            program.deleteWebsiteFromUser();
            expectEqual(program.list().size(), 3, "Program deletes website from user input.");
            expect(contains(output.text(), "Website deleted successfully."), "Program reports successful delete.");
        }

        expect(!program.exitRequested(), "Program starts without exit requested.");
        program.requestExit();
        expect(program.exitRequested(), "Program records exit request.");

        std::remove(path.c_str());
    }
}

int main()
{
    try
    {
        testWebsiteModel();
        testWebsiteListDataStructure();
        testFileLoadingLayer();
        testDisplayLayer();
        testMenuAndCommandLayer();
        testControllerLayer();

        std::cout << "All " << testsRun << " tests passed.\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Test failed: " << error.what() << '\n';
        return 1;
    }
}
