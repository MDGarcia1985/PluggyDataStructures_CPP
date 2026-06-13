/*
 * File: TargetDataStructureMenu.h
 * Description: Declares navigation for registered target-list commands.
 */

#pragma once

namespace llb
{
    class TargetProgram;

    class TargetDataStructureMenu
    {
    public:
        static void run(TargetProgram& program);
    };
}
