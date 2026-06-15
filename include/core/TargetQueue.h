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
        Target peek() const;
        bool isEmpty() const;
        std::size_t size() const;
        void display() const;
    };
}
