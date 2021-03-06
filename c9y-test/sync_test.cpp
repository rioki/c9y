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

// NOTE: Don't get smart, death tests don't work over thread boundaries.

TEST(sync, sync_throw_terminates)
{
    c9y::set_main_thread_id(std::this_thread::get_id());
    c9y::sync(std::this_thread::get_id(), [&] () {
        throw std::runtime_error("YOLO");
    });
    EXPECT_DEATH(c9y::sync_point(), "YOLO");
}

TEST(sync, sync_main_throw_terminates)
{
    c9y::set_main_thread_id(std::this_thread::get_id());
    c9y::sync([&] () {
        throw std::runtime_error("YOLO");
    });
    EXPECT_DEATH(c9y::sync_point(), "YOLO");
}

TEST(sync, delay_throw_terminates)
{
    c9y::delay([&] () {
        throw std::runtime_error("YOLO");
    });
    EXPECT_DEATH(c9y::sync_point(), "YOLO");
}
