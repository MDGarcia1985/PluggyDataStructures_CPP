/*
 * File: SelectionSort.cpp
 * Description: Registers and runs selection sort on shared cleaned electronics data.
 */

#include "ElectronicsSortSupport.h"
#include "Menu.h"


// named container llb = linked list browser
namespace llb
{
    namespace
    {
        /*
         * Purpose: Sort a Target array with selection sort.
         * Design: Receives data already loaded and cleaned by ElectronicsSortSupport.
         * Workflow: Find the smallest remaining item and swap it into the current position.
         * Data Handoff: Mutates the prepared vector supplied by the shared timing workflow.
         */
        void selectionSort(std::vector<Target>& targets)
        {
            for (std::size_t i = 0; i < targets.size(); ++i)
            {
                std::size_t minimumIndex = i;

                for (std::size_t j = i + 1; j < targets.size(); ++j)
                {
                    if (electronicsTargetLess(targets[j], targets[minimumIndex]))
                    {
                        minimumIndex = j;
                    }
                }

                if (minimumIndex != i)
                {
                    std::swap(targets[i], targets[minimumIndex]);
                }
            }
        }

        void electronicsSelectionSortCommand(TargetProgram& program)
        {
            runElectronicsSortCommand(program, "selection sort", selectionSort);
        }
    }
}

LLB_REGISTER_COMMAND(9, "Load electronics CSV and time selection sort", llb::electronicsSelectionSortCommand)
