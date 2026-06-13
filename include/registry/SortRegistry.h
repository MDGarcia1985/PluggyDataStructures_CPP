/*
 * File: SortRegistry.h
 * Description: Declares sorting algorithm registration and ordering.
 */

#pragma once

#include "registry/RegistryBase.h"


// named container llb = linked list browser
namespace llb
{
    class TargetProgram;

    struct SortCommand
    {
        std::string label;
        std::function<void(TargetProgram&)> action;
        bool isExit = false;
    };

    class SortRegistry : private RegistryBase<SortCommand>
    {
    public:
        static SortRegistry& instance();

        bool registerCommand(SortCommand command);
        std::vector<SortCommand> commands() const;

    private:
        SortRegistry();
    };
}

#define LLB_REGISTER_SORT(LABEL, FUNCTION)                                             \
    namespace                                                                          \
    {                                                                                  \
        const bool LLB_DETAIL_CONCAT(llb_registered_sort_, __LINE__) =                 \
            ::llb::SortRegistry::instance().registerCommand(                           \
                ::llb::SortCommand{LABEL, FUNCTION});                                  \
    }
