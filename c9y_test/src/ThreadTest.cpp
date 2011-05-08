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

#include <stdexcept>
#include <UnitTest++/UnitTest++.h>
#include <sigc++/sigc++.h>
#include <unistd.h>

#include "Thread.h"
#include "utility.h"

SUITE(ThreadTest)
{
//------------------------------------------------------------------------------
    struct ExecTest
    {
        bool was_executed;

        ExecTest()
        : was_executed(false) {}

        void exec()
        {
            was_executed = true;
        }
    };

//------------------------------------------------------------------------------
    void dummy_func() {}

//------------------------------------------------------------------------------
    void loop_inifinite()
    {
        while (true) {}
    }

//------------------------------------------------------------------------------
    TEST(executes)
    {
        ExecTest exec_test;
        c9y::Thread thread(sigc::mem_fun(exec_test, &ExecTest::exec));
        thread.start();
        thread.join();
        CHECK(exec_test.was_executed);
    }

//------------------------------------------------------------------------------
    TEST(start_twice_throws)
    {
        c9y::Thread thread(sigc::ptr_fun(&dummy_func));
        thread.start();
        CHECK_THROW(thread.start(), std::logic_error);
    }

//------------------------------------------------------------------------------
    TEST(kill_thread)
    {
        c9y::Thread thread(sigc::ptr_fun(&loop_inifinite));
        thread.start();
        c9y::sleep(1);
        thread.kill();
    }

//------------------------------------------------------------------------------
    TEST(kill_thread_in_destructor)
    {
        c9y::Thread thread(sigc::ptr_fun(&loop_inifinite));
        thread.start();
        c9y::sleep(1);
    }
}
