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

namespace c9y
{
//------------------------------------------------------------------------------
    DWORD WINAPI threadproc(LPVOID data)
    {
        Thread* thread = reinterpret_cast<Thread*>(data);
        thread->slot();
        return 0;
    }

//------------------------------------------------------------------------------
    Thread::Thread(sigc::slot<void> s)
    : slot(s), handle(NULL), pid(0) {}

//------------------------------------------------------------------------------
    Thread::~Thread()
    {
        if (handle != NULL)
        {
            kill();
        }
    }

//------------------------------------------------------------------------------
    void Thread::start()
    {
        if (handle != NULL)
        {
            throw std::logic_error("Trying to start a thread twice.");
        }
        handle = CreateThread(NULL, 0, &threadproc, this, 0, &pid);
        if (handle == NULL)
        {
            throw std::runtime_error("Failed to create thread.");
        }
    }

//------------------------------------------------------------------------------
    void Thread::join()
    {
        if (handle == NULL)
        {
            throw std::logic_error("Trying to join unstarted thread.");
        }

        DWORD result = WaitForSingleObject(handle, INFINITE);
        CloseHandle(handle);
        handle = NULL;
        pid = 0;

        if (result != WAIT_OBJECT_0)
        {
            throw std::runtime_error("Failed to join thread.");
        }
    }

//------------------------------------------------------------------------------
    void Thread::kill()
    {
        if (handle == NULL)
        {
            throw std::logic_error("Trying to kill unstarted thread.");
        }

        TerminateThread(handle, 2);
        CloseHandle(handle);
        handle = NULL;
        pid = 0;
    }
}

