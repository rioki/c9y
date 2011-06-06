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

#ifndef _C9Y_MUTEX_H_
#define _C9Y_MUTEX_H_

#include "config.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _POSIX
#include <pthread.h>
#endif

namespace c9y
{
    /**
     * Mututal Exclusion Lock
     *
     * The Mutex class implements a simple mutual exclusion lock. It is the
     * simplest syncronisation construct available. It synconises access to a
     * resource, that can be accessed only from one thread. The locking can be
     * done recusevly from the same thread. Locking and unlocking must be done
     * from the same thread.
     *
     * @see Lock
     **/
    class Mutex
    {
    public:
        /**
         * Constructor
         **/
        Mutex();

        /**
         * Destructor
         **/
        ~Mutex();

        /**
         * Lock the mutex.
         **/
        void lock();

        /**
         * Unlock the mutex,
         **/
        void unlock();

    private:
        #ifdef _WIN32
        HANDLE handle;
        #endif
        
        #ifdef _POSIX
        pthread_mutex_t handle;
        #endif

        Mutex(const Mutex&);
        const Mutex& operator = (const Mutex&);
    };
}

#endif

