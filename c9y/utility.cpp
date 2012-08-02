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

#include "utility.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _POSIX
#include <unistd.h>
#endif

namespace c9y
{
    void sleep(unsigned int ms)
    {
        #ifdef _WIN32
        Sleep(ms);
        #endif
        
        #ifdef _POSIX
        usleep(ms * 1000);
        #endif
    }
}

