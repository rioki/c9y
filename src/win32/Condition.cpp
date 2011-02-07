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
