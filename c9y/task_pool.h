//
// c9y - concurrency
// Copyright(c) 2017-2019 Sean Farrell
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

#ifndef _C9Y_TASK_POOL_H_
#define _C9Y_TASK_POOL_H_

#include <mutex>
#include <condition_variable>
#include <atomic>

#include "defines.h"
#include "thread_pool.h"
#include "queue.h"

namespace c9y
{
    /**
     * Task Pool
     *
     * A task pool is a wrapper around a thread pool that allow heterogenous task execution.
     *
     * There are two streams of execution, one asyncrounous and one syncronous.
     * The so called asnycronous stream is executed on the thread pool. The
     * syncronous stream is executed on the main thread. This allows to build
     * execution patterns where something is done on the thread pool and once
     * compleated synced back with the global state in the main thread, without
     * aditionall locking.
     *
     * @see async
     **/
    class C9Y_EXPORT task_pool
    {
    public:
        /**
         * Construct a task pool.
         *
         * @param concurency the number of thread to create
         **/
        task_pool(size_t concurency = std::thread::hardware_concurrency());

        task_pool(const task_pool&) = delete;

        /**
         * Destructor
         **/
        ~task_pool();

        task_pool& operator = (const task_pool&) = delete;

        /**
         * Schedule task in the asyncronous execution stream.
         **/
        void async(std::function<void ()> task);

        /**
         * Schedule task in the syncronous execution stream.
         **/
        void sync(std::function<void()> task);

        /**
         * Execute all pending tasks.
         *
         * This method will run until there are no tasks to execute.
         * Tasks can shedule further tasks, which are then executed in turn.
         *
         * @warning run, run_once and start need to be called from the same thread.
         **/
        void run();

        /**
         * Execute one sync task.
         *
         * This method will execute exactly one task from the syncronous stream.
         * When integrating in windowing systems and similar execution enviroments
         * the taks pool's main thread may be needed for other task and
         * sync tasks should only be done 'on idle'.
         *
         * @warning run, run_once and start need to be called from the same thread.
         *
         * @warnign If only run_once is used, the task pool must be joined
         * before it is being desroyed.
         **/
        void run_once();

        /**
         * Start asyncronous execution of tasks.
         *
         * This method will asyncronous execution of stats. It may be advantages
         * to start the task execution of the asyncronous stream before comitting
         * the main thread. For example for the use of the algorithms or
         * just starting to work on tasks while still setting things up.
         *
         * @warning run, run_once and start need to be called from the same thread.
         **/
        void start();

        /**
         * Join the task pool
         *
         * This method will join the underling thread pool. This is nessesary
         * if run was never called.
         **/
        void join();

    private:
        size_t                       concurency;
        std::atomic<unsigned int>    ref_count;

        queue<std::function<void()>> atasks;
        queue<std::function<void()>> stasks;

        thread_pool                  pool;

        void execute();
    };
}

#endif
