
#include "Mutex.h"

#include <cassert>
#include <stdexcept>

namespace c9y
{
//------------------------------------------------------------------------------
    Mutex::Mutex()
    {
        handle = CreateMutex(NULL, FALSE, NULL);
        if (handle == NULL)
        {
            throw std::runtime_error("Failed to create mutex.");
        }
    }

//------------------------------------------------------------------------------
    Mutex::~Mutex()
    {
        assert(handle != NULL);
        CloseHandle(handle);
    }

//------------------------------------------------------------------------------
    void Mutex::lock()
    {
        assert(handle != NULL);
        WaitForSingleObject(handle, INFINITE);
    }

//------------------------------------------------------------------------------
    void Mutex::unlock()
    {
        assert(handle != NULL);
        ReleaseMutex(handle);
    }

}

