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

#include "task_pool.h"

#include <iostream>

using namespace std::literals::chrono_literals;

namespace c9y
{
    task_pool::task_pool(size_t concurency)
    : pool([this] () {thread_func();}, concurency) {}

    task_pool::~task_pool()
    {
        running = false;
        tasks.wake();
        pool.join();
    }

    void task_pool::enqueue(const std::function<void ()>& func)
    {
        tasks.push(func);
    }

    void task_pool::thread_func()
    {
        std::function<void()> task;
        while (running)
        {
            if (tasks.pop_wait_for(task, 100ms))
            {
                try
                {
                    task();
                }
                catch (const std::exception& ex)
                {
                    std::cerr << ex.what() << std::endl;
                    std::terminate();
                }
                catch (...)
                {
                    std::terminate();
                }
            }
        }
    }
}
