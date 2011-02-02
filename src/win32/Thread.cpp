
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

