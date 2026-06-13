#pragma once

#include "core/TargetList.h"

namespace llb
{
    class TargetQueue
    {
    private:
        TargetList queue_;

    public:
        void enqueue(const Target& target);
        Target dequeue();
        bool isEmpty() const;
        void display() const;
    };
}
