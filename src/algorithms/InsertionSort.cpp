/*
 * File: InsertionSort.cpp
 * Description: Registers and runs insertion sort on shared cleaned electronics data.
 */

#include "ElectronicsSortSupport.h"
#include "Menu.h"


// named container llb = linked list browser
namespace llb
{
    namespace
    {
        /*
         * Purpose: Sort a Target array with insertion sort.
         * Design: Receives data already loaded and cleaned by ElectronicsSortSupport.
         * Workflow: Shift larger items right, then insert the saved item into place.
         * Data Handoff: Mutates the prepared vector supplied by the shared timing workflow.
         */
        void insertionSort(std::vector<Target>& targets)
        {
            for (std::size_t i = 1; i < targets.size(); ++i)
            {
                Target current = targets[i];
                std::size_t j = i;

                while (j > 0 && electronicsTargetLess(current, targets[j - 1]))
                {
                    targets[j] = targets[j - 1];
                    --j;
                }

                targets[j] = current;
            }
        }

        void electronicsInsertionSortCommand(TargetProgram& program)
        {
            runElectronicsSortCommand(program, "insertion sort", insertionSort);
        }
    }
}

LLB_REGISTER_COMMAND(8, "Load electronics CSV and time insertion sort", llb::electronicsInsertionSortCommand)
