
#include <c9y/Thread.h>
#include <c9y/utility.h>

#include <stdexcept>
#include <UnitTest++/UnitTest++.h>
#include <sigc++/sigc++.h>
#include <unistd.h>

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

//------------------------------------------------------------------------------
    TEST(join_unstarted_thread_throws)
    {
        c9y::Thread thread(sigc::ptr_fun(&dummy_func));
        CHECK_THROW(thread.join(), std::logic_error);
    }

//------------------------------------------------------------------------------
    TEST(kill_unstarted_thread_throws)
    {
        c9y::Thread thread(sigc::ptr_fun(&dummy_func));
        CHECK_THROW(thread.kill(), std::logic_error);
    }
}
