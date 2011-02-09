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

#ifndef _C9Y_THREADS_H_
#define _C9Y_THREADS_H_

#include "config.h"

#include <sigc++/slot.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _POSIX
#include <pthread.h>
#endif

namespace c9y
{
    /**
     * Thread of Execution
     *
     * The Thread class implements a wrapper around the thread concept.
     **/
    class Thread
    {
    public:
        /**
         * Constructor
         *
         * @param slot The slot that will be executed in the other thread.
         *
         * @note The constructor will not start thread execution use start() to
         * start thread execution.
         **/
        Thread(sigc::slot<void> slot);

        /**
         * Destructor
         *
         * @warning If the thread is still executing, it will be killed. You
         * whould wait (Thread::wait) for the thread to properly finish
         * execution.
         **/
        ~Thread();

        /**
         * Start the thread.
         **/
        void start();

        /**
         * Wait for the thread to finish execution.
         *
         * @todo rename to wait
         **/
        void join();

        /**
         * Forcefully stop the thread execution.
         *
         * This method will instruct the operating system to stop the execution
         * of the thread. This is a very servere operation and will probably
         * result in loss of data, since the stack is not properly unwound. You
         * should use other means to signal the thread to end and wait for it's
         * proper termination.
         **/
        void kill();

    private:
        sigc::slot<void> slot;
        
        #ifdef _WIN32
        HANDLE handle;
        DWORD pid;
        friend DWORD WINAPI threadproc(LPVOID lpParameter);
        #endif
        
        #ifdef _POSIX
        // NOTE: There is no way to check if the pthread_t value is valid or not.
        // That is there is no defined invalid value. To aliviate this we use this
        // boolean, not let's all pray that this works as expected. 
        // -> i.e. no race conditions 
        bool valid;
        pthread_t thread;
        
        friend void* threadproc(void* data); 
        #endif

        Thread(const Thread&);
        const Thread& operator = (const Thread&);
    };
}

#endif
