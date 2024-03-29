//
// c9y - concurrency
// Copyright 2017-2023 Sean Farrell
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

#include <c9y/resetting_latch.h>

#include <atomic>
#include <gtest/gtest.h>

#include <c9y/async.h>
#include <c9y/thread_pool.h>

using namespace std::chrono_literals;

TEST(resetting_latch, wait)
{
    auto my_latch = c9y::resetting_latch{12u};
    auto count    = std::atomic<unsigned int>{0u};

    for (auto i = 0u; i < 12; i++)
    {
        c9y::async([&] () {
            std::this_thread::sleep_for(50ms);
            count++;
            my_latch.count_down();
        });
    }

    // There is a subtle race condition between count_down and wait,
    // that std::latch does not have. You need to make sure your condition
    // you are waiting on is not yet achived. If that is the case, don't wait
    // since the latch is now in it's next iteration.
    if (count != 12u)
    {
        my_latch.wait();
    }
    EXPECT_EQ(12u, count);
}

TEST(resetting_latch, wait_multiple)
{
    auto my_latch = c9y::resetting_latch{12u};
    auto count    = std::atomic<unsigned int>{0u};

    for (auto i = 0u; i < 12; i++)
    {
        c9y::async([&] () {
            count++;
            my_latch.count_down();
        });
    }

    my_latch.wait();
    EXPECT_EQ(12u, count);

    for (auto i = 0u; i < 12; i++)
    {
        c9y::async([&] () {
            count++;
            my_latch.count_down();
        });
    }

    my_latch.wait();
    EXPECT_EQ(24u, count);
}
TEST(resetting_latch, arrive_and_wait)
{
    auto my_latch = c9y::resetting_latch{12u};
    auto count    = std::atomic<unsigned int>{0u};

    auto js = c9y::thread_pool([&] () {
            count++;
            my_latch.arrive_and_wait();
    }, 11u);

    my_latch.arrive_and_wait();
    EXPECT_EQ(11u, count);
}

TEST(resetting_latch, arrive_and_wait_multiple)
{
    auto my_latch = c9y::resetting_latch{12u};
    auto count    = std::atomic<unsigned int>{0u};

    auto js = c9y::thread_pool([&] () {
            count++;
            my_latch.arrive_and_wait();

            EXPECT_EQ(11u, count);
            my_latch.arrive_and_wait();

            count++;
            my_latch.arrive_and_wait();
    }, 11u);

    my_latch.arrive_and_wait();
    EXPECT_EQ(11u, count);
    my_latch.arrive_and_wait();

    my_latch.arrive_and_wait();
    EXPECT_EQ(22u, count);
}
