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

#include <UnitTest++/UnitTest++.h>

#include "Timer.h"
#include "utility.h"

SUITE(Timer)
{
//------------------------------------------------------------------------------
    struct TimerFixture
    {
        unsigned int tick_count;
        
        TimerFixture()
        : tick_count(0) {}
        
        void on_tick()
        {
            tick_count++;
        }
    };

//------------------------------------------------------------------------------    
    TEST_FIXTURE(TimerFixture, run_time)
    {
        c9y::Timer timer(20, sigc::mem_fun(this, &TimerFixture::on_tick));
        timer.start();
        c9y::sleep(110);
        timer.stop();
        
        CHECK_EQUAL(5, tick_count);
    }
}