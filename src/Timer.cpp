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

#include "Timer.h"

#include <iostream>

#include "Lock.h"
#include "utility.h"

namespace c9y
{
//------------------------------------------------------------------------------
    Timer::Timer(unsigned int i, sigc::slot<void> s)
    : interval(i), slot(s), thread(sigc::mem_fun(this, &Timer::thread_func)) {}

//------------------------------------------------------------------------------    
    Timer::~Timer() 
    {
        stop();
    }
    
//------------------------------------------------------------------------------
    void Timer::start()
    {
        thread.start();
    }
    
//------------------------------------------------------------------------------
    void Timer::stop()
    {
        Lock<Mutex> lock(exec_mutex);
        thread.kill();
    }
    
//------------------------------------------------------------------------------
    void Timer::thread_func()
    {
        while (true)
        {
            sleep(interval);
            {
                Lock<Mutex> lock(exec_mutex);
                try
                {
                    slot();
                }
                catch (const std::exception& ex)
                {
                    std::cerr << ex.what() << std::endl;
                }
            }
        }
    }
}
