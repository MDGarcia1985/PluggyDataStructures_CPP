#pragma once

#include "core/TargetList.h"

namespace llb
{
    class TargetStack
    {
    private:
        TargetList stack_;

    public:
        void push(const Target& target);
        Target pop();
        Target peek() const;
        bool isEmpty() const;
        std::size_t size() const;
        void display() const;
    };
}
