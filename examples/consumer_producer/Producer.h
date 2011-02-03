
#ifndef _COPR_PRODUCER_H_
#define _COPR_PRODUCER_H_

#include <c9y/Thread.h>

namespace copr
{
    class Queue;

    class Producer
    {
    public:

        Producer(Queue& queue);

        void start();

        void finish();

    private:
        Queue& queue;

        c9y::Thread thread;
        bool running;

        void production_loop();
    };
}

#endif
