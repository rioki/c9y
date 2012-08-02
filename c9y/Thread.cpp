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
    #ifdef _WIN32
    DWORD WINAPI threadproc(LPVOID data)
    {
        Thread* thread = reinterpret_cast<Thread*>(data);
        try
        {
            thread->slot();
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
        }
        return 0;
    }
    #endif
    
//------------------------------------------------------------------------------
    #ifdef _POSIX
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
    #endif

//------------------------------------------------------------------------------
    Thread::Thread(sigc::slot<void> s)
    : slot(s), 
    #ifdef _WIN32
    handle(NULL), pid(0)
    #endif
    #ifdef _POSIX
    valid(false)
    #endif
    {}

//------------------------------------------------------------------------------
    Thread::~Thread()
    {
        #ifdef _WIN32
        if (handle != NULL)
        {
            kill();
        }
        #endif
        
        #ifdef _POSIX
        if (valid)
        {
            kill();
        }
        #endif
    }

//------------------------------------------------------------------------------
    void Thread::start()
    {
        #ifdef _WIN32
        if (handle != NULL)
        {
            throw std::logic_error("Trying to start a thread twice.");
        }
        handle = CreateThread(NULL, 0, &threadproc, this, 0, &pid);
        if (handle == NULL)
        {
            throw std::runtime_error("Failed to create thread.");
        }
        #endif
        
        #ifdef _POSIX
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
        #endif
    }

//------------------------------------------------------------------------------
    void Thread::join()
    {
        #ifdef _WIN32
        if (handle == NULL)
        {
            return;
        }

        DWORD result = WaitForSingleObject(handle, INFINITE);
        CloseHandle(handle);
        handle = NULL;
        pid = 0;

        if (result != WAIT_OBJECT_0)
        {
            throw std::runtime_error("Failed to join thread.");
        }
        #endif
        
        #ifdef _POSIX
        if (!valid)
        {
            return;
        }

        int ret = pthread_join(thread, NULL);
        valid = false;
        
        if (ret != 0)
        {
            throw std::runtime_error("Failed to join thread.");
        }
        #endif
    }

//------------------------------------------------------------------------------
    void Thread::kill()
    {
        #ifdef _WIN32
        if (handle == NULL)
        {
            return;
        }

        TerminateThread(handle, 2);
        CloseHandle(handle);
        handle = NULL;
        pid = 0;
        #endif
        
        #ifdef _POSIX
        if (!valid)
        {
            return;
        }
        pthread_cancel(thread);
        valid = false;
        #endif
    }
}

