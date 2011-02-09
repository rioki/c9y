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

#include "Consumer.h"

#include <iostream>
#include <cstdlib>
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
