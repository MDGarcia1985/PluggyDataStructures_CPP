/*
 * File: Menu.h
 * Description: Declares menu prompting plus the command plugin registry and macro.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Website: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Header.h"


// named container llb = linked list browser
namespace llb
{
    class WebsiteProgram;

    struct CommandPlugin
    {
        int id;
        std::string label;
        std::function<void(WebsiteProgram&)> action;
    };

    class CommandRegistry
    {
    public:
        static CommandRegistry& instance();

        bool registerCommand(CommandPlugin command);
        std::vector<CommandPlugin> commands() const;
        const CommandPlugin* findById(int id) const;

    private:
        CommandRegistry() = default;
        std::vector<CommandPlugin> commands_;
    };

    class Menu
    {
    public:
        static void display(const std::vector<CommandPlugin>& commands);
        static int promptChoice();
        static int promptInteger(const std::string& prompt);
    };
}

#define LLB_DETAIL_CONCAT_IMPL(first, second) first##second
#define LLB_DETAIL_CONCAT(first, second) LLB_DETAIL_CONCAT_IMPL(first, second)

// Register a command from any .cpp file compiled into the project.
// New plugin authors only need to include App.h and use this macro.
#define LLB_REGISTER_COMMAND(ID, LABEL, FUNCTION)                                      \
    namespace                                                                          \
    {                                                                                  \
        const bool LLB_DETAIL_CONCAT(llb_registered_command_, __LINE__) =              \
            ::llb::CommandRegistry::instance().registerCommand(                        \
                ::llb::CommandPlugin{ID, LABEL, FUNCTION});                            \
    }
