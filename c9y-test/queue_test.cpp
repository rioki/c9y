//
// c9y - concurrency
// Copyright 2017-2022 Sean Farrell
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

#include <c9y/queue.h>

#include <atomic>
#include <thread>

#include <gtest/gtest.h>
#include <c9y/thread_pool.h>

using namespace std::chrono_literals;

TEST(queue, create)
{
    auto q = c9y::queue<int>{};
}

TEST(queue, consumer_producer)
{
    auto q = c9y::queue<int>{};
    auto count = std::atomic<unsigned int>{0};

    auto prod = c9y::thread_pool{[&] () {
        for (int i = 1; i < 101; i++)
        {
            q.push(i);
        }
    }, 3};

    auto cons = c9y::thread_pool{[&] () {
        while (auto value = q.pop())
        {
            count++;
            std::this_thread::sleep_for(1ms);
        }
    }, 3};

    prod.join();
    cons.join();

    EXPECT_EQ(300, static_cast<unsigned int>(count));
}

TEST(queue, consumer_producer_wait)
{
    auto q = c9y::queue<int>{};
    auto count = std::atomic<unsigned int>{0};

    auto prod = c9y::thread_pool{[&] () {
        for (int i = 1; i < 101; i++)
        {
            q.push(i);
            std::this_thread::sleep_for(5ms);
        }
    }, 3};

    auto cons = c9y::thread_pool{[&] () {
        while (auto value = q.pop_wait_for(100ms))
        {
            count++;
            std::this_thread::sleep_for(3ms);
        }
    }, 3};

    prod.join();
    std::this_thread::sleep_for(100ms);
    q.stop();
    cons.join();

    EXPECT_EQ(300, static_cast<unsigned int>(count));
}
