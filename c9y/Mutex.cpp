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

#include "Mutex.h"

#include <cassert>
#include <stdexcept>

namespace c9y
{
//------------------------------------------------------------------------------
    Mutex::Mutex()
    {
        #ifdef _WIN32
        handle = CreateMutex(NULL, FALSE, NULL);
        if (handle == NULL)
        {
            throw std::runtime_error("Failed to create mutex.");
        }
        #endif
        
        #ifdef _POSIX
        pthread_mutexattr_t attr;
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

        int ret = pthread_mutex_init(&handle, &attr);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to create mutex.");
        }
        #endif
    }

//------------------------------------------------------------------------------
    Mutex::~Mutex()
    {
        #ifdef _WIN32
        assert(handle != NULL);
        CloseHandle(handle);
        #endif
        
        #ifdef _POSIX
        pthread_mutex_destroy(&handle);
        #endif
    }

//------------------------------------------------------------------------------
    void Mutex::lock()
    {
        #ifdef _WIN32
        assert(handle != NULL);
        WaitForSingleObject(handle, INFINITE);
        #endif
        
        #ifdef _POSIX
        int ret = pthread_mutex_lock(&handle);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to lock mutex.");
        }
        #endif
    }

//------------------------------------------------------------------------------
    void Mutex::unlock()
    {
        #ifdef _WIN32
        assert(handle != NULL);
        ReleaseMutex(handle);
        #endif
        
        #ifdef _POSIX
        int ret = pthread_mutex_unlock(&handle);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to unlock mutex.");
        }
        #endif
    }

}

