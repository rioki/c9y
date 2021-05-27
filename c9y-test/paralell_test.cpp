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

#include <c9y/paralell.h>

#include <atomic>
#include <array>
#include <gtest/gtest.h>

TEST(paralell, paralell_container)
{
    auto count = std::atomic<unsigned int>{0};
    auto coutner = std::array<std::function<void()>, 4>{
        [&]() {
            count += 1;
        },
        [&]() {
            count += 3;
        },
        [&]() {
            count += 3;
        },
        [&]() {
            count += 7;
        },
    };

    c9y::paralell(begin(coutner), end(coutner));
    EXPECT_EQ(14, static_cast<unsigned int>(count));
}

TEST(paralell, paralell_inplace)
{
    auto count = std::atomic<unsigned int>{0};
    c9y::paralell({
        [&]() {
            count += 1;
        },
        [&]() {
            count += 3;
        },
        [&]() {
            count += 3;
        },
        [&]() {
            count += 7;
        },
    });
    EXPECT_EQ(14, static_cast<unsigned int>(count));
}