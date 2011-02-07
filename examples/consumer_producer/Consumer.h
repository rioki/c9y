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
