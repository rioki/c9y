
#include "Condition.h"

#include <stdexcept>

namespace c9y
{
//------------------------------------------------------------------------------
    Condition::Condition()
    : thread_count(0)
    {
        handle = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (handle == NULL)
        {
            throw std::runtime_error("Failed to create condition.");
        }
    }

//------------------------------------------------------------------------------
    Condition::~Condition()
    {
        CloseHandle(handle);
    }

//------------------------------------------------------------------------------
    void Condition::wait()
    {
        thread_count++;

        DWORD result = WaitForSingleObject(handle, INFINITE);

        if (--thread_count == 0)
        {
            ResetEvent(handle);
        }

        if (result != WAIT_OBJECT_0)
        {
            throw std::runtime_error("Failed to whait on condition.");
        }
    }

//------------------------------------------------------------------------------
    void Condition::signal()
    {
        BOOL result = SetEvent(handle);
        if (result == FALSE)
        {
            throw std::runtime_error("Failed to signal condition.");
        }
    }
}
