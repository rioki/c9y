
#include "Consumer.h"

#include <iostream>
#include <c9y/utility.h>

#include "Queue.h"

namespace copr
{
//------------------------------------------------------------------------------
    Consumer::Consumer(Queue& q)
    : queue(q), thread(sigc::mem_fun(this, &Consumer::consumption_loop)), running(false) {}

//------------------------------------------------------------------------------
    void Consumer::start()
    {
        running = true;
        thread.start();
    }

//------------------------------------------------------------------------------
    void Consumer::finish()
    {
        running = false;
        thread.join();
    }

//------------------------------------------------------------------------------
    void Consumer::consumption_loop()
    {
        while (running)
        {
            c9y::sleep(rand() % 10);
            unsigned int value = queue.pop();
            std::cout << queue.get_size() << ":" << value << std::endl;
        }
    }
}
