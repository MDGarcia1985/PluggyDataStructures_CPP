/*
 * File: ElectronicsSortSupport.h
 * Description: Declares shared electronics data preparation and sort command support.
 */

#pragma once

#include "TargetProgram.h"


// named container llb = linked list browser
namespace llb
{
    using ElectronicsSortFunction = void (*)(std::vector<Target>&);

    bool electronicsTargetLess(const Target& left, const Target& right);

    void runElectronicsSortCommand(
        TargetProgram& program,
        const std::string& algorithmName,
        ElectronicsSortFunction sortFunction);
}
