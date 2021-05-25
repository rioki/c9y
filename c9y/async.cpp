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

#include "async.h"

#include "thread_pool.h"
#include "queue.h"

using namespace std::literals::chrono_literals;

namespace c9y
{
    class AsyncWorker
    {
    public:
        AsyncWorker() noexcept = default;

        ~AsyncWorker()
        {
            running = false;
            task_queue.wake();
            pool.join();
        }

        void shedule(const std::function<void()>& func) noexcept
        {
            task_queue.push(func);
        }

    private:
        std::atomic<bool>            running    = true;
        queue<std::function<void()>> task_queue;
        thread_pool                  pool       = thread_pool([this] () {thread_func();}, std::thread::hardware_concurrency());

        void thread_func()
        {
            std::function<void()> task;
            while (running)
            {
                if (task_queue.pop_wait_for(task, 100ms))
                {
                    task();
                }
            }
        }
    };
    AsyncWorker worker;

    void async(const std::function<void()>& func) noexcept
    {
        worker.shedule(func);
    }
}
