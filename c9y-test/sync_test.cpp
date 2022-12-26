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

#include <c9y/sync.h>

#include <gtest/gtest.h>

using namespace std::literals::chrono_literals;
TEST(sync, empty_tick)
{
    c9y::sync_point();
}

TEST(sync, set_main_thread)
{
    c9y::set_main_thread_id(std::this_thread::get_id());
    EXPECT_EQ(std::this_thread::get_id(), c9y::get_main_thread_id());
}

TEST(sync, sync_different_thread)
{
    std::thread::id exec_id;

    std::thread thread_a([&] () {
        while (exec_id == std::thread::id())
        {
            std::this_thread::sleep_for(50ms);
            c9y::sync_point();

        }
    });

    std::thread thread_b([&] () {
        c9y::sync(thread_a.get_id(), [&] () {
            exec_id = std::this_thread::get_id();
        });
    });

    auto id_a = thread_a.get_id();
    thread_a.join();
    thread_b.join();

    EXPECT_EQ(id_a, exec_id);
}
TEST(sync, sync_main_thread)
{
    std::thread::id exec_id;
    c9y::set_main_thread_id(std::this_thread::get_id());

    std::thread worker([&] () {
        c9y::sync([&] () {
            exec_id = std::this_thread::get_id();
        });
    });

    while (exec_id == std::thread::id())
    {
        std::this_thread::sleep_for(50ms);
        c9y::sync_point();

    }

    worker.join();

    EXPECT_EQ(std::this_thread::get_id(), exec_id);
}

TEST(sync, sync_different_thread_with_result)
{
    std::thread::id exec_id;
    unsigned int result = 0;

    std::thread thread_a([&] () {
        while (exec_id == std::thread::id())
        {
            std::this_thread::sleep_for(50ms);
            c9y::sync_point();

        }
    });

    std::thread thread_b([&] () {
        auto f = c9y::sync<unsigned int>(thread_a.get_id(), [&] () {
            exec_id = std::this_thread::get_id();
            return 42;
        });
        result = f.get();
    });

    auto id_a = thread_a.get_id();
    thread_a.join();
    thread_b.join();

    EXPECT_EQ(id_a, exec_id);
    EXPECT_EQ(42, result);
}

TEST(sync, sync_main_thread_with_result)
{
    std::thread::id exec_id;
    unsigned int result = 0;
    c9y::set_main_thread_id(std::this_thread::get_id());

    std::thread worker([&] () {
        auto f = c9y::sync<unsigned int>([&] () {
            exec_id = std::this_thread::get_id();
            return 42;
        });
        result = f.get();
    });

    while (exec_id == std::thread::id())
    {
        std::this_thread::sleep_for(50ms);
        c9y::sync_point();
    }

    worker.join();

    EXPECT_EQ(std::this_thread::get_id(), exec_id);
    EXPECT_EQ(42, result);
}

TEST(sync, delay)
{
    unsigned int result = 0;

    c9y::delay([&] () {
        result = 42;
    });

    while (result == 0)
    {
        std::this_thread::sleep_for(50ms);
        c9y::sync_point();
    }

    EXPECT_EQ(42, result);
}

TEST(sync, delay_once)
{
    auto ot    = c9y::once_tag{};
    auto count = 0u;

    c9y::delay(ot, [&] () {
        count++;
    });
    c9y::delay(ot, [&] () {
        count++;
    });

    EXPECT_EQ(0u, count);
    c9y::sync_point();
    EXPECT_EQ(1u, count);

    c9y::delay(ot, [&] () {
        count++;
    });
    c9y::delay(ot, [&] () {
        count++;
    });
    c9y::delay(ot, [&] () {
        count++;
    });

    EXPECT_EQ(1u, count);
    c9y::sync_point();
    EXPECT_EQ(2u, count);
}

TEST(sync, sync_fun_main_thread)
{
    c9y::set_main_thread_id(std::this_thread::get_id());

    auto count = 0u;
    auto func  = c9y::sync_fun([&] () {
        count++;
    });

    auto done = false;
    auto worker = std::jthread([&] () {
        func();
        func();
        func();
        c9y::sync([&] () {
            done = true;
        });
    });

    while (!done)
    {
        c9y::sync_point();
    }

    EXPECT_EQ(3u, count);
}

TEST(sync, sync_fun_different_thread)
{
    auto consumer = std::jthread([&] (std::stop_token st) {
        while (!st.stop_requested())
        {
            c9y::sync_point();
        }
    });

    auto count = 0u;
    auto func  = c9y::sync_fun(consumer.get_id(), [&] () {
        count++;
    });

    auto producer = std::jthread([&] () {
        func();
        func();
        func();
    });

    producer.join();
    consumer.request_stop();
    consumer.join();

    EXPECT_EQ(3u, count);
}

TEST(sync, sync_fun_once_main_thread)
{
    c9y::set_main_thread_id(std::this_thread::get_id());

    auto ot    = c9y::once_tag{};
    auto count = 0u;
    auto func  = c9y::sync_fun(ot, [&] () {
        count++;
    });

    auto done = false;
    auto worker = std::jthread([&] () {
        func();
        func();
        func();
        c9y::sync([&] () {
            done = true;
        });
    });

    while (!done)
    {
        c9y::sync_point();
    }

    EXPECT_EQ(1u, count);
}

TEST(sync, sync_fun_once_different_thread)
{
    auto consumer = std::jthread([&] (std::stop_token st) {
        std::this_thread::sleep_for(50ms); // let some time pass so all work is injected.
        do
        {
            c9y::sync_point();
        }
        while (!st.stop_requested());
    });

    auto ot    = c9y::once_tag{};
    auto count = 0u;
    auto func  = c9y::sync_fun(ot, consumer.get_id(), [&] () {
        count++;
    });

    auto producer = std::jthread([&] () {
        func();
        func();
        func();
    });

    producer.join();
    consumer.request_stop();
    consumer.join();

    EXPECT_EQ(1u, count);
}
