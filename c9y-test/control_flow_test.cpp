// 
// c9y - concurrency
// Copyright(c) 2017 Sean Farrell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <atomic>
#include <ctime>
#include <iostream>

#include <c9y/c9y.h>

#include "rtest.h"

SUITE(control_flow)
{
    TEST(async)
    {
        unsigned int result = 0;
        std::thread::id atid, stid;

        c9y::task_pool pool;

        c9y::async<unsigned int>(pool, [&]() -> unsigned int {
            atid = std::this_thread::get_id();
            return 5;
        }, [&](std::exception_ptr err, unsigned int r) {
            stid = std::this_thread::get_id();
            result = r;
        });

        pool.run();

        CHECK(std::this_thread::get_id() != atid);
        CHECK(std::this_thread::get_id() == stid);
        CHECK_EQUAL(5, result);
    }

    TEST(async_exception)
    {
        std::string err_msg;

        c9y::task_pool pool;

        c9y::async<unsigned int>(pool, [&]() -> unsigned int {
            throw std::runtime_error("A Problem");
        }, [&](std::exception_ptr err, unsigned int r) {
            try
            {
                std::rethrow_exception(err);
            }
            catch (std::runtime_error& ex)
            {
                err_msg = ex.what();
            }
        });

        pool.run();

        CHECK_EQUAL("A Problem", err_msg);
    }

    TEST(async_void)
    {
        std::thread::id atid, stid;

        c9y::task_pool pool;

        c9y::async(pool, [&]() {
            atid = std::this_thread::get_id();
        }, [&](std::exception_ptr err) {
            stid = std::this_thread::get_id();
        });

        pool.run();

        CHECK(std::this_thread::get_id() != atid);
        CHECK(std::this_thread::get_id() == stid);
    }

    TEST(async_void_exception)
    {
        std::string err_msg;

        c9y::task_pool pool;

        c9y::async(pool, [&]() {
            throw std::runtime_error("A Void Problem");
        }, [&](std::exception_ptr err) {
            try
            {
                std::rethrow_exception(err);
            }
            catch (std::runtime_error& ex)
            {
                err_msg = ex.what();
            }
        });

        pool.run();

        CHECK_EQUAL("A Void Problem", err_msg);
    }

    TEST(idle)
    {
        unsigned int result = 0;
        std::thread::id stid;

        c9y::task_pool pool;

        c9y::handle handle;
        handle = c9y::start_idle(pool, [&] () -> bool {
            stid = std::this_thread::get_id();
            result++;
            return (result < 5);
        });

        pool.run();
        CHECK(std::this_thread::get_id() == stid);
        CHECK_EQUAL(5, result);
    }

    TEST(timer)
    {
        CHECK_EQUAL(1000, CLOCKS_PER_SEC);

        unsigned int result = 0;
        unsigned int start, end;
        std::thread::id stid;

        c9y::task_pool pool;

        c9y::timer(pool, [&]() {
            stid = std::this_thread::get_id();
            result++;
            return (result < 5);
        }, 15);

        start = c9y::get_ms_time();
        pool.run();
        end = c9y::get_ms_time();

        unsigned int diff = end - start;
        
        CHECK_EQUAL(5, result);
        CHECK(74 < diff);
        CHECK(std::this_thread::get_id() == stid);
    }

    TEST(async_and_idle)
    {
        bool done = false;

        c9y::task_pool pool;

        auto ih = c9y::start_idle(pool, [&] () {
            // something stupid
            return true;
        });

        c9y::async(pool, [&]() {
            // nothing
        }, [&](std::exception_ptr err) {
            c9y::stop_idle(ih);
            done = true;
        });

        pool.run();

        CHECK(done);
    }
}
