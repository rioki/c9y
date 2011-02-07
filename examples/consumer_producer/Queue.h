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
