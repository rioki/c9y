
#include "Queue.h"

#include <c9y/Lock.h>

namespace copr
{
//------------------------------------------------------------------------------
    Queue::Queue()
    {
        queue_not_full.signal();
        queue_not_empty.clear();
    }

//------------------------------------------------------------------------------
    void Queue::push(unsigned int value)
    {
        queue_not_full.wait();

        c9y::Lock<c9y::Mutex> lock(mutex);
        queue.push(value);

        if (queue.size() == 5)
        {
            queue_not_full.clear();
        }
        queue_not_empty.signal();
    }

//------------------------------------------------------------------------------
    unsigned int Queue::pop()
    {
        queue_not_empty.wait();

        c9y::Lock<c9y::Mutex> lock(mutex);
        unsigned int value = queue.front();
        queue.pop();

        if (queue.size() == 0)
        {
            queue_not_empty.clear();
        }
        queue_not_full.signal();

        return value;
    }

//------------------------------------------------------------------------------
    unsigned int Queue::get_size() const
    {
        c9y::Lock<c9y::Mutex> lock(mutex);
        return queue.size();
    }
}
