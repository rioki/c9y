
#include <c9y/Mutex.h>
#include <c9y/Lock.h>
#include <c9y/Thread.h>

#include <UnitTest++/UnitTest++.h>


SUITE(MutexTest)
{
//------------------------------------------------------------------------------
    class Account
    {
    public:
        Account()
        : total(1000) {}

        unsigned int get_total() const
        {
            return total;
        }

        void set_total(unsigned int value)
        {
            total = value;
        }

        void lock()
        {
            mutex.lock();
        }

        void unlock()
        {
            mutex.unlock();
        }

    private:
        c9y::Mutex mutex;
        unsigned int total;
    };

//------------------------------------------------------------------------------
#ifdef _WIN32
    void sleep(unsigned int ms)
    {
        Sleep(ms);
    }
#endif

//------------------------------------------------------------------------------
    class DepositTask
    {
    public:

        DepositTask(Account& a)
        : account (a) {}

        void operator () ()
        {
            c9y::Lock<Account> lock(account);
            unsigned int total = account.get_total();
            sleep(100);
            total += 200;
            account.set_total(total);
        }

    private:
        Account& account;
    };

//------------------------------------------------------------------------------
    TEST(basic_locking)
    {
        Account account;

        DepositTask task1(account);
        c9y::Thread thread1(task1);

        DepositTask task2(account);
        c9y::Thread thread2(task2);

        thread1.start();
        thread2.start();

        thread1.join();
        thread2.join();

        CHECK_EQUAL(1400, account.get_total());
    }
}
