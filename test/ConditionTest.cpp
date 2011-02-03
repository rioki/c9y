
#include <c9y/Condition.h>
#include <c9y/Thread.h>
#include <c9y/utility.h>

#include <cassert>
#include <UnitTest++/UnitTest++.h>

SUITE(ConditionTest)
{
//------------------------------------------------------------------------------
    class Countdown
    {
    public:
        Countdown()
        : value(100) {}

        unsigned int get_value() const
        {
            return value;
        }

        void tick()
        {
            value--;
            if (value == 0)
            {
                condition.signal();
            }
        }

        void wait_launch()
        {
            condition.wait();
        }

    private:
        c9y::Condition condition;
        unsigned int value;
    };

//------------------------------------------------------------------------------
    class CountTask
    {
    public:
        CountTask(Countdown& c)
        : countdown(c) {}

        void operator () ()
        {
            while (countdown.get_value() != 0)
            {
                countdown.tick();
                c9y::sleep(10);
            }
        }

    private:
        Countdown& countdown;
    };

//------------------------------------------------------------------------------
    class LaunchTask
    {
    public:
        LaunchTask(Countdown& c)
        : countdown(c), launched(false) {}

        void exec()
        {
            countdown.wait_launch();
            assert(countdown.get_value() == 0);
            launched = true;
        }

        bool was_launched()
        {
            return launched;
        }

    private:
        Countdown& countdown;
        bool launched;
    };

    TEST(countdown)
    {
        Countdown countdown;

        CountTask task1(countdown);
        c9y::Thread thread1(task1);

        LaunchTask task2(countdown);
        c9y::Thread thread2(sigc::mem_fun(task2, &LaunchTask::exec));

        LaunchTask task3(countdown);
        c9y::Thread thread3(sigc::mem_fun(task3, &LaunchTask::exec));

        thread1.start();
        thread2.start();
        thread3.start();

        thread1.join();
        thread2.join();
        thread3.join();

        CHECK(task2.was_launched());
        CHECK(task3.was_launched());
        CHECK_EQUAL(0, countdown.get_value());
    }
}
