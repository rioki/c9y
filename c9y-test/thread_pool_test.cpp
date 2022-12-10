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

#include <c9y/thread_pool.h>

#include <atomic>
#include <gtest/gtest.h>

using namespace std::chrono_literals;

TEST(thread_pool, create)
{
    auto count = std::atomic<unsigned int>{0};

    auto pool = c9y::thread_pool{[&]() {
        count++;
    }, 2};

    EXPECT_EQ(2, pool.get_concurency());

    pool.join();

    EXPECT_EQ(2, static_cast<unsigned int>(count));
}

TEST(thread_pool, default_contructor)
{
    auto pool = c9y::thread_pool{};
    EXPECT_EQ(0, pool.get_concurency());
}

TEST(thread_pool, default_concurency)
{
    auto pool = c9y::thread_pool{[&]() {}};
    EXPECT_EQ(std::thread::hardware_concurrency(), pool.get_concurency());
}

TEST(thread_pool, move)
{
    auto count = std::atomic<unsigned int>{0};
    auto pool = c9y::thread_pool{};

    pool = c9y::thread_pool{[&]() {
        count++;
    }, 2};
    pool.join();

    EXPECT_EQ(2, static_cast<unsigned int>(count));
}

TEST(thread_pool, no_join)
{
    auto pool = c9y::thread_pool{[] {}};
}

#ifdef __cpp_lib_jthread
TEST(thread_pool, stop_rquest)
{
    auto pool = c9y::thread_pool{[&] (std::stop_token st) {
        while (!st.stop_requested())
        {
            std::this_thread::sleep_for(10ms);
        }
    }};

    EXPECT_TRUE(pool.request_stop());
}
#endif
