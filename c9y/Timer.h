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

#ifndef _C9Y_TIMER_H_
#define _C9Y_TIMER_H_

#include "config.h"

#include "Thread.h"
#include "Mutex.h"

namespace c9y
{
    /**
     * Callback Timer 
     **/
    class Timer
    {
    public:
        /**
         * Constructor
         *
         * @param inverval the interval to call the slot
         * @param slot the slot to call
         **/
        Timer(unsigned int inverval, sigc::slot<void> slot);
        
        ~Timer();
        
        /**
         * Start the timer. 
         **/
        void start();
        
        /**
         * Stop the timer.
         **/
        void stop();
        
    private:
        unsigned int interval;
        sigc::slot<void> slot;
        Thread thread;
        Mutex exec_mutex;
        
        void thread_func();
        
        Timer(const Timer&);
        const Timer& operator = (const Timer&);
    };
}

#endif
