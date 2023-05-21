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

#include <c9y/c9y.h>

#include <gtest/gtest.h>

TEST(exceptions, unhandled_exception_handler)
{
    auto count = 0u;
    auto old_handler = c9y::set_unhandled_exception([&] () {
        count++;
    });

    c9y::unhandled_exception();
    EXPECT_EQ(1u, count);

    c9y::set_unhandled_exception(old_handler);
}

TEST(exceptions, sync_throws)
{
    auto count = 0u;
    auto old_handler = c9y::set_unhandled_exception([&] () {
        count++;
    });

    c9y::sync(std::this_thread::get_id(), [&] () {
        throw std::runtime_error("YOLO");
    });
    c9y::sync_point();
    EXPECT_EQ(1u, count);

    c9y::set_unhandled_exception(old_handler);
}

TEST(exceptions, sync_main_throws)
{
    auto count = 0u;
    auto old_handler = c9y::set_unhandled_exception([&] () {
        count++;
    });
    c9y::set_main_thread_id(std::this_thread::get_id());
    c9y::sync([&] () {
        throw std::runtime_error("YOLO");
    });

    c9y::sync_point();
    EXPECT_EQ(1u, count);

    c9y::set_unhandled_exception(old_handler);
}

TEST(exceptions, delay_throws)
{
    auto count = 0u;
    auto old_handler = c9y::set_unhandled_exception([&] () {
        count++;
    });

    c9y::delay([&] () {
        throw std::runtime_error("YOLO");
    });

    c9y::sync_point();
    EXPECT_EQ(1u, count);

    c9y::set_unhandled_exception(old_handler);
}

TEST(exceptions, taks_pool)
{
    auto count = 0u;
    auto old_handler = c9y::set_unhandled_exception([&] () {
        count++;
    });

    auto pool = c9y::task_pool(3);

    pool.enqueue([] () {
        throw std::runtime_error("one");
    });
    pool.enqueue([] () {
        throw std::runtime_error("two");
    });
    pool.enqueue([] () {
        throw std::runtime_error("three");
    });

    pool.flush();
    EXPECT_EQ(3u, count);

    c9y::set_unhandled_exception(old_handler);
}
