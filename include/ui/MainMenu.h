/*
 * File: MainMenu.h
 * Description: Declares the top-level application menu flow.
 */

#pragma once

namespace llb
{
    class TargetProgram;

    class MainMenu
    {
    public:
        static void run(TargetProgram& program);
    };
}
