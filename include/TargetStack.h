#pragma once

#include "TargetList.h"

namespace llb
{
    class TargetStack
    {
    private:
        TargetList stack_;

    public:
        void push(const Target& target);
        Target pop();
        bool isEmpty() const;
        void display() const;
    };
}
