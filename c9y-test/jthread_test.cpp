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

#include <c9y/c9y.h>

#include <gtest/gtest.h>

TEST(jthread, create_and_destroy)
{
    auto thread = c9y::jthread{};
}

TEST(jthread, launch_thread)
{
    auto tid = c9y::jthread::id{};
    auto thread = c9y::jthread{[&] () {
        tid = std::this_thread::get_id();
    }};

    thread.join();
    EXPECT_NE(tid, std::this_thread::get_id());
}

TEST(jthread, launch_thread_with_args)
{
    auto a = 0u;
    auto thread = c9y::jthread{[&] (unsigned int awnser) {
        a = awnser;
    }, 42u};

    thread.join();
    EXPECT_EQ(a, 42u);
}

TEST(jthread, id)
{
    auto latch = c9y::latch{1};
    auto tid = c9y::jthread::id{};
    auto thread = c9y::jthread{[&] () {
        tid = std::this_thread::get_id();
        latch.count_down();
    }};

    latch.wait();
    EXPECT_EQ(thread.get_id(), tid);
    thread.join();
}

TEST(jthread, empty_id)
{
    auto thread = c9y::jthread{};
    EXPECT_EQ(thread.get_id(), c9y::jthread::id{});
}

TEST(jthread, detach)
{
    auto latch = c9y::latch{1};

    {
        auto thread = c9y::jthread{[&] () {
            latch.wait();
        }};
        thread.detach();
    }

    latch.count_down();
}

TEST(jthread, move_assignment)
{
    auto latch = c9y::latch{1};
    auto outside = c9y::jthread{};
    {
        auto inside = c9y::jthread{[&] () {
            latch.wait();
        }};
        outside = std::move(inside);
        EXPECT_FALSE(inside.joinable());
    }

    EXPECT_TRUE(outside.joinable());
    latch.count_down();
    outside.join();
}

TEST(jthread, move_constructor)
{
    auto latch = c9y::latch{1};

    auto construct = [&] () {
        auto inside = c9y::jthread{[&] () {
            latch.wait();
        }};
        return inside;
    };
    auto outside = construct();

    EXPECT_TRUE(outside.joinable());
    latch.count_down();
    outside.join();
}

TEST(jthread, swap)
{
    auto latch = c9y::latch{1};
    auto a = c9y::jthread{};
    auto b = c9y::jthread{[&] () {
        latch.wait();
    }};

    EXPECT_FALSE(a.joinable());
    EXPECT_TRUE(b.joinable());
    a.swap(b);
    EXPECT_TRUE(a.joinable());
    EXPECT_FALSE(b.joinable());

    latch.count_down();
    a.join();
}

TEST(jthread, request_stop)
{
    auto thread = c9y::jthread{[] (c9y::stop_token token) {
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
    }};

    thread.request_stop();
    thread.join();
}

TEST(jthread, automatic_stop_request)
{
    auto thread = c9y::jthread{[] (c9y::stop_token token) {
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
    }};
}

TEST(jthread, stop_callback)
{
    auto should_be_called     = std::atomic<unsigned int>{0};
    auto should_not_be_called = std::atomic<unsigned int>{0};

    auto worker = c9y::jthread{[] (c9y::stop_token token) {
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
    }};

    auto callback = c9y::stop_callback(worker.get_stop_token(), [&] {
        should_be_called++;
    });

    {
        auto scoped_callback = c9y::stop_callback(worker.get_stop_token(), [&] {
            should_not_be_called++;
        });
    }

    auto stopper_func = [&] {
        worker.request_stop();
    };

    c9y::jthread stopper1(stopper_func);
    c9y::jthread stopper2(stopper_func);
    stopper1.join();
    stopper2.join();

    EXPECT_EQ(1, should_be_called);
    EXPECT_EQ(0, should_not_be_called);

    auto should_be_called_imediatly = std::atomic<unsigned int>{0};
    auto callback2 = c9y::stop_callback(worker.get_stop_token(), [&] {
        should_be_called_imediatly++;
    });

    EXPECT_EQ(1, should_be_called);
    EXPECT_EQ(0, should_not_be_called);
    EXPECT_EQ(1, should_be_called_imediatly);
}

TEST(jthread, called_stop_callback)
{
    auto worker = c9y::jthread{[] (c9y::stop_token token) {
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
    }};

    worker.request_stop();

    auto should_be_called_imediatly = std::atomic<unsigned int>{0};
    auto callback2 = c9y::stop_callback(worker.get_stop_token(), [&] {
        should_be_called_imediatly++;
    });

    EXPECT_EQ(1, should_be_called_imediatly);
}

TEST(jthread, expired_stop_callback)
{
    auto worker = c9y::jthread{[] (c9y::stop_token token) {
        while (!token.stop_requested())
        {
            std::this_thread::yield();
        }
    }};

    worker.request_stop();
    worker.join();

    auto should_be_called_imediatly = std::atomic<unsigned int>{0};
    auto callback2 = c9y::stop_callback(worker.get_stop_token(), [&] {
        should_be_called_imediatly++;
    });

    EXPECT_EQ(1, should_be_called_imediatly);
}

