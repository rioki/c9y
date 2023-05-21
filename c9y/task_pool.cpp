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

#include "task_pool.h"

#include "exceptions.h"

using namespace std::literals::chrono_literals;

namespace c9y
{
    task_pool::task_pool(size_t concurency) noexcept
    : pool([this] () {thread_func();}, concurency) {}

    task_pool::~task_pool()
    {
        tasks.stop();
        tasks_in_flight = 0; // needed to release flush
        flush_cv.notify_all();
    }

    void task_pool::enqueue(const std::function<void ()>& func) noexcept
    {
        tasks_in_flight++;
        tasks.push(func);
    }

    void task_pool::flush() noexcept
    {
        auto lock = std::unique_lock<std::mutex>{flush_mutex};

        if (tasks_in_flight == 0)
        {
            return;
        }

        flush_cv.wait(lock, [&]{return tasks_in_flight == 0;});
    }

    void task_pool::thread_func() noexcept
    {
        while (auto task = tasks.pop_wait())
        {
            try
            {
                (*task)();
            }
            catch (...)
            {
                c9y::unhandled_exception();
            }

            if (--tasks_in_flight == 0)
            {
                flush_cv.notify_all();
            }
        }
    }
}
