
#include "Condition.h"

#include <stdexcept>

namespace c9y
{
//------------------------------------------------------------------------------
    Condition::Condition()
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
        DWORD result = WaitForSingleObject(handle, INFINITE);
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

//------------------------------------------------------------------------------
    void Condition::clear()
    {
        BOOL result = ResetEvent(handle);
        if (result == FALSE)
        {
            throw std::runtime_error("Failed to clear condition.");
        }
    }
}
