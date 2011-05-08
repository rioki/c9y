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

#include "Mutex.h"
#include "Lock.h"
#include "Thread.h"
#include "utility.h"

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
    class DepositTask
    {
    public:

        DepositTask(Account& a)
        : account (a) {}

        void operator () ()
        {
            c9y::Lock<Account> lock(account);
            unsigned int total = account.get_total();
            c9y::sleep(100);
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
