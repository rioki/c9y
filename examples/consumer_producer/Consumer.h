
#ifndef _COPR_CONSUMER_H_
#define _COPR_CONSUMER_H_

#include <c9y/Thread.h>

namespace copr
{
    class Queue;

    class Consumer
    {
    public:

        Consumer(Queue& queue);

        void start();

        void finish();

    private:
        Queue& queue;

        c9y::Thread thread;
        bool running;

        void consumption_loop();
    };
}

#endif
