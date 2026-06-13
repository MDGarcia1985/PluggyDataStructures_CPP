/*
 * File: ElectronicsSortSupport.cpp
 * Description: Prepares cleaned electronics data and runs shared sort timing workflows.
 */

#include "ElectronicsSortSupport.h"

#include <chrono>
#include <iomanip>


// named container llb = linked list browser
namespace llb
{
    namespace
    {
        struct ComponentSortKey
        {
            std::string prefix;
            long long number;
            bool hasNumber;
        };

        /*
         * Purpose: Remove leading and trailing whitespace from CSV fields.
         * Design: Centralizes electronics field cleanup for every sorting algorithm.
         * Workflow: Find the first and last non-whitespace character and return that substring.
         * Data Handoff: Receives raw CSV text and returns cleaned text used to build Target records.
         */
        std::string trim(const std::string& text)
        {
            const std::string whitespace = " \t\n\r";
            const std::size_t first = text.find_first_not_of(whitespace);

            if (first == std::string::npos)
            {
                return "";
            }

            const std::size_t last = text.find_last_not_of(whitespace);
            return text.substr(first, last - first + 1);
        }

        /*
         * Purpose: Split one CSV row into cleaned fields.
         * Design: Supports quoted fields and doubled quote escapes in one shared parser.
         * Workflow: Walk the row, toggle quote state, split outside quotes, and trim each field.
         * Data Handoff: Converts a raw file line into normalized fields for row validation.
         */
        std::vector<std::string> parseCsvLine(const std::string& line)
        {
            std::vector<std::string> fields;
            std::string field;
            bool insideQuotes = false;

            for (std::size_t i = 0; i < line.size(); ++i)
            {
                const char current = line[i];

                if (current == '"')
                {
                    if (insideQuotes && i + 1 < line.size() && line[i + 1] == '"')
                    {
                        field.push_back('"');
                        ++i;
                    }
                    else
                    {
                        insideQuotes = !insideQuotes;
                    }
                }
                else if (current == ',' && !insideQuotes)
                {
                    fields.push_back(trim(field));
                    field.clear();
                }
                else
                {
                    field.push_back(current);
                }
            }

            fields.push_back(trim(field));
            return fields;
        }

        /*
         * Purpose: Append a named component attribute to the generic second Target field.
         * Design: Skips empty values so cleaned output stays readable for sparse rows.
         * Workflow: Add a separator when needed, then append the label/value pair.
         * Data Handoff: Receives one cleaned CSV value and contributes it to Target::fieldTwo().
         */
        void appendAttribute(std::string& output, const std::string& label, const std::string& value)
        {
            if (value.empty())
            {
                return;
            }

            if (!output.empty())
            {
                output += "; ";
            }

            output += label;
            output += '=';
            output += value;
        }

        /*
         * Purpose: Convert one cleaned electronics row into the existing two-field Target shape.
         * Design: Uses ref_des as fieldOne and combines the remaining useful attributes in fieldTwo.
         * Workflow: Validate the row, compose its detail field, and return whether a Target was produced.
         * Data Handoff: Appends normalized electronics data to the shared clean target vector.
         */
        bool addComponentRow(const std::vector<std::string>& row, std::vector<Target>& targets)
        {
            if (row.size() < 9 || row[0] == "circuit_name" || row[2].empty())
            {
                return false;
            }

            std::string details;
            appendAttribute(details, "pin", row[7]);
            appendAttribute(details, "net", row[8]);
            appendAttribute(details, "type", row[4]);

            std::string value = row[5];
            if (!row[6].empty())
            {
                if (!value.empty())
                {
                    value += ' ';
                }
                value += row[6];
            }
            appendAttribute(details, "value", value);

            appendAttribute(details, "kind", row[3]);
            appendAttribute(details, "circuit", row[0]);
            appendAttribute(details, "description", row[1]);

            targets.emplace_back(row[2], details);
            return true;
        }

        /*
         * Purpose: Load and clean the electronics sample CSV once for any sorting algorithm.
         * Design: Returns an array of Target values so algorithms receive prepared data only.
         * Workflow: Open the first matching path, parse valid rows, normalize fields, and report the path.
         * Data Handoff: Supplies a clean vector shared by insertion, selection, and future sorts.
         */
        std::vector<Target> loadCleanElectronicsData(std::string& loadedPath)
        {
            const std::vector<std::string> candidatePaths = {
                std::string(DATA_DIRECTORY) + "circuit_sample_two.csv",
                "circuit_sample_two.csv",
                "../data/circuit_sample_two.csv"
            };

            std::ifstream inputFile;
            for (const std::string& path : candidatePaths)
            {
                inputFile.open(path);
                if (inputFile.is_open())
                {
                    loadedPath = path;
                    break;
                }
                inputFile.clear();
            }

            std::vector<Target> targets;
            if (!inputFile.is_open())
            {
                return targets;
            }

            std::string line;
            while (std::getline(inputFile, line))
            {
                if (!trim(line).empty())
                {
                    addComponentRow(parseCsvLine(line), targets);
                }
            }

            return targets;
        }

        /*
         * Purpose: Create a natural key from a reference designator such as R12 or C3.
         * Design: Separates leading letters from following digits so numeric portions sort numerically.
         * Workflow: Uppercase leading letters, collect following digits, and return both parts.
         * Data Handoff: Supplies electronicsTargetLess() with a stable component ordering key.
         */
        ComponentSortKey makeComponentSortKey(const std::string& refDes)
        {
            ComponentSortKey key{"", 0, false};
            std::size_t index = 0;

            while (index < refDes.size() && std::isalpha(static_cast<unsigned char>(refDes[index])))
            {
                key.prefix.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(refDes[index]))));
                ++index;
            }

            while (index < refDes.size() && std::isdigit(static_cast<unsigned char>(refDes[index])))
            {
                key.hasNumber = true;
                key.number = key.number * 10 + (refDes[index] - '0');
                ++index;
            }

            return key;
        }

        std::size_t repetitionsForSize(std::size_t size)
        {
            if (size < 100)
            {
                return 1000;
            }
            if (size < 1000)
            {
                return 100;
            }
            if (size < 5000)
            {
                return 20;
            }
            return 3;
        }

        void replaceListWithSortedArray(TargetList& targets, const std::vector<Target>& sortedTargets)
        {
            targets.clear();
            for (const Target& target : sortedTargets)
            {
                targets.addBack(target);
            }
        }
    }

    /*
     * Purpose: Compare cleaned electronics Target records in natural component order.
     * Design: Shares one strict ordering across every sorting algorithm.
     * Workflow: Compare prefix, numeric part, raw fieldOne, then fieldTwo.
     * Data Handoff: Receives two prepared targets and returns their relative order.
     */
    bool electronicsTargetLess(const Target& left, const Target& right)
    {
        const ComponentSortKey leftKey = makeComponentSortKey(left.fieldOne());
        const ComponentSortKey rightKey = makeComponentSortKey(right.fieldOne());

        if (leftKey.prefix != rightKey.prefix)
        {
            return leftKey.prefix < rightKey.prefix;
        }

        if (leftKey.hasNumber && rightKey.hasNumber && leftKey.number != rightKey.number)
        {
            return leftKey.number < rightKey.number;
        }

        if (left.fieldOne() != right.fieldOne())
        {
            return left.fieldOne() < right.fieldOne();
        }

        return left.fieldTwo() < right.fieldTwo();
    }

    /*
     * Purpose: Run the shared cleaned-data and timing workflow around one sorting algorithm.
     * Design: Keeps algorithm files focused only on sorting behavior and command registration.
     * Workflow: Load clean data, clone arrays, time sorting, replace the list, and report results.
     * Data Handoff: Passes prepared arrays into the selected sort function and writes back its result.
     */
    void runElectronicsSortCommand(
        TargetProgram& program,
        const std::string& algorithmName,
        ElectronicsSortFunction sortFunction)
    {
        std::string loadedPath;
        const std::vector<Target> baseline = loadCleanElectronicsData(loadedPath);

        if (baseline.empty())
        {
            std::cout << "\nCould not load electronics data from data/circuit_sample_two.csv.\n";
            return;
        }

        const std::size_t repetitions = repetitionsForSize(baseline.size());
        std::vector<std::vector<Target>> timedArrays(repetitions, baseline);

        const auto startTime = std::chrono::steady_clock::now();
        for (std::vector<Target>& timedArray : timedArrays)
        {
            sortFunction(timedArray);
        }
        const auto endTime = std::chrono::steady_clock::now();

        const std::chrono::duration<double> elapsedSeconds = endTime - startTime;
        const double totalSeconds = elapsedSeconds.count();
        const double averageSeconds = totalSeconds / static_cast<double>(repetitions);

        replaceListWithSortedArray(program.list(), timedArrays.front());

        std::cout << std::fixed << std::setprecision(9);
        std::cout << "\nElectronics CSV loaded: " << baseline.size() << " row(s) from " << loadedPath << ".\n";
        std::cout << "Sort algorithm: " << algorithmName << "\n";
        std::cout << "Array size: " << baseline.size() << "\n";
        std::cout << "Sort repetitions: " << repetitions << "\n";
        std::cout << "Total sort time: " << totalSeconds << " seconds\n";
        std::cout << "Average sort time: " << averageSeconds << " seconds\n";
        std::cout << "Linked list replaced with the sorted electronics records.\n";
    }
}
