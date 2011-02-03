
#ifndef _COPR_QUEUE_H_
#define _COPR_QUEUE_H_

#include <queue>
#include <c9y/Mutex.h>
#include <c9y/Condition.h>

namespace copr
{
    class Queue
    {
    public:

        Queue();

        void push(unsigned int value);

        unsigned int pop();

        unsigned int get_size() const;

    private:
        mutable c9y::Mutex mutex;
        c9y::Condition queue_not_empty;
        c9y::Condition queue_not_full;
        std::queue<unsigned int> queue;
    };
}

#endif
