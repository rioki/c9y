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

#include "Producer.h"

#include <cstdlib>
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
