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

#include <c9y/defer.h>

#include <gtest/gtest.h>

TEST(defer, empty_catch_up)
{
    EXPECT_FALSE(c9y::catch_up());
}

TEST(defer, shedule_one_task)
{
    auto count = 0u;
    c9y::defer([&] () {
        count++;
    });
    EXPECT_EQ(count, 0u);

    EXPECT_TRUE(c9y::catch_up());
    EXPECT_EQ(count, 1u);

    EXPECT_FALSE(c9y::catch_up());
    EXPECT_EQ(count, 1u);
}

TEST(defer, shedule_multiple_tasks)
{
    auto count = 0u;
    c9y::defer([&] () {
        count++;
    });
    c9y::defer([&] () {
        count++;
    });
    c9y::defer([&] () {
        count++;
    });
    EXPECT_EQ(count, 0u);

    c9y::catch_up();
    EXPECT_EQ(count, 3u);

    c9y::catch_up();
    EXPECT_EQ(count, 3u);
}

TEST(defer, null_defer_throws)
{
    EXPECT_THROW(c9y::defer({}), std::invalid_argument);
}


