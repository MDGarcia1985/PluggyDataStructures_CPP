/*
 * File: SortSupport.cpp
 * Description: Implements generic comparison, timing, and sorted-list replacement.
 */

#include "sorting/SortSupport.h"

#include <chrono>
#include <iomanip>


// named container llb = linked list browser
namespace llb
{
    namespace
    {
        /*
         * Purpose: Normalize text for case-insensitive generic ordering.
         * Design: Creates a lowercase copy so stored Target values remain unchanged.
         * Workflow: Copy the text, lowercase each byte safely, and return the normalized value.
         * Data Handoff: Supplies targetLess() with comparable text derived from Target fields.
         */
        std::string lowercase(std::string text)
        {
            std::transform(text.begin(), text.end(), text.begin(),
                [](unsigned char character)
                {
                    return static_cast<char>(std::tolower(character));
                });
            return text;
        }

        /*
         * Purpose: Choose a practical timing repetition count from the active dataset size.
         * Design: Uses fewer repetitions for larger quadratic sorting workloads.
         * Workflow: Compare the size against thresholds and return the matching count.
         * Data Handoff: Supplies runSortCommand() with a benchmark loop count.
         */
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

        /*
         * Purpose: Replace a linked list with a sorted vector snapshot.
         * Design: Rebuilds through TargetList's public API so node ownership remains encapsulated.
         * Workflow: Clear the list and append every sorted Target in order.
         * Data Handoff: Moves sorted values from algorithm storage back into the active TargetList.
         */
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
     * Purpose: Compare generic Target records for ascending sort order.
     * Design: Uses case-insensitive field-one ordering, then field two, with raw text as a stable tie-breaker.
     * Workflow: Normalize fields, compare primary and secondary values, then compare original display text.
     * Data Handoff: Receives two Targets from an algorithm and returns their strict relative ordering.
     */
    bool targetLess(const Target& left, const Target& right)
    {
        const std::string leftFirst = lowercase(left.fieldOne());
        const std::string rightFirst = lowercase(right.fieldOne());
        if (leftFirst != rightFirst)
        {
            return leftFirst < rightFirst;
        }

        const std::string leftSecond = lowercase(left.fieldTwo());
        const std::string rightSecond = lowercase(right.fieldTwo());
        if (leftSecond != rightSecond)
        {
            return leftSecond < rightSecond;
        }

        return left.toDisplayString() < right.toDisplayString();
    }

    /*
     * Purpose: Apply and time a registered sorting algorithm on the currently loaded dataset.
     * Design: Keeps data preparation and list replacement outside individual algorithm modules.
     * Workflow: Snapshot the list, clone benchmark inputs, time each sort, replace the list, and report.
     * Data Handoff: Passes prepared vectors to the algorithm and writes the first sorted result into the program.
     */
    void runSortCommand(
        TargetProgram& program,
        const std::string& algorithmName,
        SortFunction sortFunction)
    {
        const std::vector<Target> baseline = program.list().toVector();
        if (baseline.empty())
        {
            std::cout << "\nThe target list is empty. Nothing was sorted.\n";
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
        std::cout << "\nSort algorithm: " << algorithmName << "\n";
        std::cout << "Array size: " << baseline.size() << "\n";
        std::cout << "Sort repetitions: " << repetitions << "\n";
        std::cout << "Total sort time: " << totalSeconds << " seconds\n";
        std::cout << "Average sort time: " << averageSeconds << " seconds\n";
        std::cout << "Linked list replaced with the sorted records.\n";
    }

}
