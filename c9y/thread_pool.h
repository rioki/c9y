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

#ifndef _C9Y_THREAD_POOL_H_
#define _C9Y_THREAD_POOL_H_

#include <functional>
#include <thread>
#include <vector>

#include "defines.h"

namespace c9y
{
    //! Thread Pool
    class C9Y_EXPORT thread_pool
    {
    public:
        //! Contruct empty thread pool.
        //!
        //! The default constructor will create a thread pool, but with no threads.
        //! It can be used to create an instance on the stack that can later accept
        //! a running thread pool with move assignment.
        thread_pool() noexcept = default;

        //! Create a thread pool.
        //!
        //! @param thread_func The thread function each thread in the pool executes.
        //! @param concurency The number of threads to run concurently.
        //!
        //! This constructor will create @arg concurency threads that execute @arg thread_func.
        template <typename Callable>
        thread_pool(Callable thread_func, size_t concurency = std::thread::hardware_concurrency())
        {
            for (size_t i = 0; i < concurency; i++)
            {
                threads.emplace_back(thread_func);
            }
        }

        //! Move Constructor
        thread_pool(thread_pool&& other) noexcept = default;

        //! Destructor
        #ifdef __cpp_lib_jthread
        ~thread_pool() = default;
        #else
        ~thread_pool();
        #endif

        //! Move Asignment
        thread_pool& operator = (thread_pool&& other) noexcept = default;

        //! Get the concurency of the thread pool.
        size_t get_concurency() const;

        //! Join each thread in the pool.
        //!
        //! Calling join ensures that each thread function terminates and
        //! the coresponsing memory is freed.
        void join();

        #ifdef __cpp_lib_jthread
        //! Request the thread pool to stop.
        //!
        //! If the thread function was initialized with a stop_token and handles
        //! it, this function will signal the stop token.
        //!
        //! @returns true if the stop request could be issues to all threads.
        bool request_stop() noexcept;
        #endif

    private:
        #ifdef __cpp_lib_jthread
        std::vector<std::jthread> threads;
        #else
        // clang, update your stdlib!
        std::vector<std::thread> threads;
        #endif

        thread_pool(const thread_pool&) = delete;
        thread_pool& operator = (const thread_pool&) = delete;
    };
}

#endif
