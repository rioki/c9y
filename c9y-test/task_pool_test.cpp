//
// c9y - concurrency
// Copyright(c) 2017-2021 Sean Farrell
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
#include <future>

#include <c9y/c9y.h>

#include <gtest/gtest.h>


TEST(task_pool, fallthrough)
{
    auto pool = c9y::task_pool{};
    pool.run();

}

TEST(task_pool, create)
{
    auto count = std::atomic<unsigned int>{0};

    auto task = [&]() {
        count++;
    };

    auto pool = c9y::task_pool{2};

    pool.async(task);
    pool.async(task);
    pool.async(task);
    pool.sync(task);

    pool.run();

    EXPECT_EQ(4, static_cast<unsigned int>(count));
}

TEST(task_pool, start_fallthrough)
{
    auto pool = c9y::task_pool{};
    pool.start();
    pool.run();
}

TEST(task_pool, run_once)
{
    auto count = std::atomic<unsigned int>{0};

    auto pool = c9y::task_pool{};
    pool.sync([&] () {
        count++;
    });
    pool.sync([&] () {
        count++;
    });

    pool.run_once();
    pool.run_once();
    pool.join();

    EXPECT_EQ(2, static_cast<unsigned int>(count));
}

TEST(task_pool, run_once_leave_work_in_queue)
{
    auto count = std::atomic<unsigned int>{0};

    auto pool = c9y::task_pool{};
    pool.sync([&] () {
        count++;
    });
    pool.sync([&] () {
        count++;
    });

    pool.run_once();
    pool.join();

    EXPECT_EQ(1, static_cast<unsigned int>(count));
}

TEST(task_pool, start_join_no_run)
{
    auto pool = c9y::task_pool{};
    pool.start();
    pool.join();
}

TEST(task_pool, start_join_no_run_still_works_async)
{
    auto promise = std::promise<void>();

    auto pool = c9y::task_pool{};
    pool.start();

    pool.async([&] () {
        promise.set_value();
    });

    auto future = promise.get_future();
    future.wait();

    pool.join();
}

