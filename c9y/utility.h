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

#ifndef _C9Y_UTILITY_H_
#define _C9Y_UTILITY_H_

#include "config.h"

namespace c9y
{
    /**
     * Suspend Execution for a Defined Duration
     *
     * This function will supend the execution of the current thread for a
     * specified duration.
     *
     * @param ms the ducration in milliseconds
     *
     * @note Depending on your operating system and environment the granularity
     * can be up 10 ms.
     **/
    void sleep(unsigned int ms);
}

#endif

