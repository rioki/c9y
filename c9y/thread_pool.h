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

#ifndef _C9Y_THREAD_POOL_H_
#define _C9Y_THREAD_POOL_H_

#include <functional>
#include <thread>
#include <vector>

#include "defines.h"

namespace c9y
{
    /**
     * Thread Pool
     **/
    class C9Y_EXPORT thread_pool
    {
    public:
        /**
         * Contruct empty thread pool.
         *
         * The default constructor will create a thread pool, but with no threads.
         * It can be used to create an instance on the stack that can later accept
         * a running thread pool with move assignment.
         **/
        thread_pool() noexcept;

        /**
         * Create a thread pool.
         *
         * @param thread_func The thread function each thread in the pool executes.
         * @param concurency The number of threads to run concurently.
         *
         * This constructor will create @arg concurency threads that execute @arg thread_func.
         **/
        thread_pool(std::function<void ()> thread_func, size_t concurency);

        thread_pool(const thread_pool&) = delete;

        /**
         * Move Constructor
         **/
        thread_pool(thread_pool&& other) noexcept;

        /**
         * Destructor
         *
         * @warning If the thread pool is not joined, the underlying std::thread
         * objects will invoke std::terminate.
         **/
        ~thread_pool();

        thread_pool& operator = (const thread_pool&) = delete;

        /**
         * Move Asignment
         **/
        thread_pool& operator = (thread_pool&& other) noexcept;

        /**
         * Get the concurency of the thread pool.
         */
        size_t get_concurency() const;

        /**
         * Join each thread in the pool.
         *
         * Calling join ensures that each thread function terminates and
         * the coresponsing memory is freed.
         **/
        void join();

        /**
         * Swap thread pool.
         **/
        void swap(thread_pool& other) noexcept;

    private:
        std::vector<std::thread> threads;
    };
}

#endif

