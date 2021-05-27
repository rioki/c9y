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

#include <c9y/async.h>

#include <future>
#include <gtest/gtest.h>

TEST(async, launch)
{
    std::promise<std::thread::id> p;
    c9y::async([&] () {
        p.set_value(std::this_thread::get_id());
    });

    auto id = p.get_future().get();
    EXPECT_NE(std::this_thread::get_id(), id);
}

TEST(async, launch_with_future)
{
    auto f = c9y::async<std::thread::id>([&] () {
        return std::this_thread::get_id();
    });

    auto id = f.get();
    EXPECT_NE(std::this_thread::get_id(), id);
}

TEST(async, exception_captured_by_future)
{
    auto throwing_func = [] () -> int {
        throw std::runtime_error("Boom!");
    };

    auto f = c9y::async<int>(throwing_func);
    EXPECT_THROW(f.get(), std::runtime_error);
}
