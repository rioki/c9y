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

#include "Thread.h"

#include <stdexcept>
#include <iostream>

namespace c9y
{
//------------------------------------------------------------------------------
    void* threadproc(void* data)
    {
        // fix the state of things to emulate the desired kill behavior
        int dummy;
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &dummy);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &dummy);
        
        Thread* thread = reinterpret_cast<Thread*>(data);
        try
        {
            thread->slot();
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
        }
        return NULL;
    }

//------------------------------------------------------------------------------
    Thread::Thread(sigc::slot<void> s)
    : slot(s), valid(false) {}

//------------------------------------------------------------------------------
    Thread::~Thread()
    {
        if (valid)
        {
            kill();
        }
    }

//------------------------------------------------------------------------------
    void Thread::start()
    {
        if (valid)
        {
            throw std::logic_error("Trying to start a thread twice.");
        }
        int ret = pthread_create(&thread, NULL, &threadproc, this);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to create thread.");
        }
        valid = true;
    }

//------------------------------------------------------------------------------
    void Thread::join()
    {
        if (!valid)
        {
            throw std::logic_error("Trying to join unstarted thread.");
        }

        int ret = pthread_join(thread, NULL);
        valid = false;
        
        if (ret != 0)
        {
            throw std::runtime_error("Failed to join thread.");
        }
    }

//------------------------------------------------------------------------------
    void Thread::kill()
    {
        if (!valid)
        {
            throw std::logic_error("Trying to kill unstarted thread.");
        }
        pthread_cancel(thread);
        valid = false;
    }
}

