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

#include <chrono>
#include <coroutine>
#include <exception>
#include <future>
#include <iostream>
#include <thread>
#include <type_traits>

#include <c9y/async.h>
#include <c9y/sync.h>
#include <c9y/coroutine.h>

#include <gtest/gtest.h>

#ifdef __cpp_lib_coroutine

std::future<int> compute_async() {
    using namespace c9y::co_async;
    int a = co_await c9y::async<int>([] { return 6; });
    int b = co_await c9y::async<int>([] { return 7; });
    co_return a * b;
}

std::future<int> compute_sync() {
    using namespace c9y::co_sync;
    int a = co_await c9y::async<int>([] { return 6; });
    int b = co_await c9y::async<int>([] { return 7; });
    co_return a * b;
}

std::future<void> fail() {
    throw std::runtime_error("fail");
    co_return;
}

TEST(coroutine, compute_async)
{
    EXPECT_EQ(6 * 7, compute_async().get());
}

TEST(coroutine, compute_sync)
{
    using namespace std::chrono_literals;

    auto f = compute_sync();

    while (f.wait_for(0s) == std::future_status::timeout)
    {
        c9y::sync_point();
    }

    EXPECT_EQ(6 * 7, f.get());
}
TEST(coroutine, fail)
{
    EXPECT_THROW(fail().get(), std::runtime_error);
}

#endif
