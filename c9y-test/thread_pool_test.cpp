// 
// c9y - concurrency
// Copyright(c) 2017 Sean Farrell
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

#include <atomic>

#include <c9y/c9y.h>

#include "rtest.h"

SUITE(thread_pool)
{
    TEST(create)
    {
        std::atomic<unsigned int> count(0);

        c9y::thread_pool pool([&]() {
            count++;
        }, 2);
        pool.join();

        CHECK_EQUAL(2, (unsigned int)count);
    }

    TEST(default_contructor)
    {
        c9y::thread_pool pool();
    }

    TEST(move)
    {
        std::atomic<unsigned int> count(0);
        c9y::thread_pool pool;

        pool = c9y::thread_pool([&]() {
            count++;
        }, 2);
        pool.join();

        CHECK_EQUAL(2, (unsigned int)count);
    }
}