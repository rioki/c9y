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
#include <thread>

#include <c9y/c9y.h>

#include "rtest.h"

using namespace std::chrono_literals;

SUITE(queue)
{
    TEST(create)
    {
        c9y::queue<int> q;
    }

    TEST(consumer_producer)
    {
        c9y::queue<int> q;
        std::atomic<unsigned int> count(0);        

        c9y::thread_pool prod([&] () {
            for (int i = 1; i < 101; i++)
            {
                q.push(i);
            }            
        }, 3);

        c9y::thread_pool cons([&] () {
            int value = 0;
            while (q.pop(value))
            {
                count++;             
            }
        }, 3);

        prod.join();
        cons.join();

        CHECK_EQUAL(300, count.load());
    }

    TEST(consumer_producer_wait)
    {
        c9y::queue<int> q;
        std::atomic<unsigned int> count(0);        

        c9y::thread_pool prod([&] () {
            for (int i = 1; i < 101; i++)
            {
                q.push(i);
                std::this_thread::sleep_for(5ms);
            }            
        }, 3);

        c9y::thread_pool cons([&] () {
            int value = 0;
            while (q.pop_wait(value))
            {
                count++;            
                std::this_thread::sleep_for(3ms); 
            }
        }, 3);

        prod.join();
        std::this_thread::sleep_for(100ms);
        q.wake();
        cons.join();

        CHECK_EQUAL(300, count.load());
    }
}