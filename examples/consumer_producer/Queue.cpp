//
// c9y - concurrency
// Copyright 2011 Sean Farell
//
// This file is part of c9y.
//
// c9y is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// c9y is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with c9y. If not, see <http://www.gnu.org/licenses/>.
//

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
