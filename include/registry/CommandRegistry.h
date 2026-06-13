/*
 * File: CommandRegistry.h
 * Description: Declares target-menu command registration and ordering.
 */

#pragma once

#include "registry/RegistryBase.h"


// named container llb = linked list browser
namespace llb
{
    class TargetProgram;

    struct CommandPlugin
    {
        int id;
        std::string label;
        std::function<void(TargetProgram&)> action;
        bool isExit = false;
    };

    class CommandRegistry : private RegistryBase<CommandPlugin>
    {
    public:
        static CommandRegistry& instance();

        bool registerCommand(CommandPlugin command);
        std::vector<CommandPlugin> commands() const;
        const CommandPlugin* findById(int id) const;

    private:
        CommandRegistry() = default;
    };
}

#define LLB_REGISTER_COMMAND(ID, LABEL, FUNCTION)                                      \
    namespace                                                                          \
    {                                                                                  \
        const bool LLB_DETAIL_CONCAT(llb_registered_command_, __LINE__) =              \
            ::llb::CommandRegistry::instance().registerCommand(                        \
                ::llb::CommandPlugin{ID, LABEL, FUNCTION});                            \
    }

#define LLB_REGISTER_EXIT_COMMAND(ID, LABEL, FUNCTION)                                 \
    namespace                                                                          \
    {                                                                                  \
        const bool LLB_DETAIL_CONCAT(llb_registered_exit_command_, __LINE__) =         \
            ::llb::CommandRegistry::instance().registerCommand(                        \
                ::llb::CommandPlugin{ID, LABEL, FUNCTION, true});                      \
    }
