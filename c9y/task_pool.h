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

#ifndef _C9Y_TASK_POOL_H_
#define _C9Y_TASK_POOL_H_

#include <functional>
#include <thread>
#include <vector>
#include <atomic>

#include "defines.h"
#include "thread_pool.h"
#include "queue.h"

namespace c9y
{
    //! Task Pool
    //!
    //! A task pool is a pool of threads that are executing heterogeneous tasks.
    class C9Y_EXPORT task_pool
    {
    public:
        //! Construct task pool with given concurrency.
        //!
        //! @param concurency the number of threads spawn.
        explicit task_pool(size_t concurency) noexcept;

        //! Destructor
        ~task_pool();

        //! Add a task to the work queue.
        void enqueue(const std::function<void ()>& func) noexcept;

        //! Wait for all pending work to clear.
        void flush() noexcept;

    private:
        queue<std::function<void ()>> tasks;
        thread_pool                   pool;

        std::atomic<unsigned int>     tasks_in_flight = 0;
        std::mutex                    flush_mutex;
        std::condition_variable       flush_cv;

        void thread_func() noexcept;

        task_pool(const thread_pool&) = delete;
        task_pool& operator = (const task_pool&) = delete;
    };
}

#endif
