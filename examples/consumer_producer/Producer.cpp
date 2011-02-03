
#include "Producer.h"

#include <c9y/utility.h>

#include "Queue.h"

namespace copr
{
//------------------------------------------------------------------------------
    Producer::Producer(Queue& q)
    : queue(q), thread(sigc::mem_fun(this, &Producer::production_loop)), running(false) {}

//------------------------------------------------------------------------------
    void Producer::start()
    {
        running = true;
        thread.start();
    }

//------------------------------------------------------------------------------
    void Producer::finish()
    {
        running = false;
        thread.join();
    }

//------------------------------------------------------------------------------
    void Producer::production_loop()
    {
        while (running)
        {
            c9y::sleep(rand() % 10);
            queue.push(rand());
        }
    }
}
