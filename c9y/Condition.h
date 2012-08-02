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

#ifndef _C9Y_CONDITION_H_
#define _C9Y_CONDITION_H_

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
     * Condition
     *
     * The Condition class implements a means of inter thread comunication to
     * signal if a given condition is met or not.
     *
     * @note The concept has more in common with Windows' events than POSIX's
     * condition variables.
     **/
    class Condition
    {
    public:
        /**
         * Constructor
         *
         * Create a condition witch is in the clear state.
         **/
        Condition();

        /**
         * Destrcutor
         **/
        ~Condition();

        /**
         * Wait on the condition to be met.
         *
         * This method will wait until the condition is met. If the condition
         * already is met, there obvioulsy will be no wait.
         **/
        void wait();

        /**
         * Signal the Condition
         *
         * This method will notify all waiting threads that the condition is
         * met and wake them up apropriately.
         **/
        void signal();

        /**
         * Clear the Condition
         *
         * This method will set the condition so it is not met. Any thread that
         * will wait on the condition will block.
         **/
        void clear();

    private:
        #ifdef _WIN32
        HANDLE handle;
        #endif
        
        #ifdef _POSIX
        bool signaled;
        pthread_mutex_t mutex;
        pthread_cond_t cond;
        #endif

        Condition(const Condition&);
        const Condition& operator = (const Condition&);
    };
}

#endif
