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

#ifndef _C9Y_THREAD_POOL_H_
#define _C9Y_THREAD_POOL_H_

#include <functional>
#include <thread>
#include <vector>

#include "defines.h"

namespace c9y
{
    class C9Y_EXPORT thread_pool
    {
    public:
        thread_pool() noexcept;

        thread_pool(std::function<void ()> thread_func, size_t concurency);

        thread_pool(const thread_pool&) = delete;

        thread_pool(thread_pool&& other) noexcept;

        ~thread_pool();

        thread_pool& operator = (const thread_pool&) = delete;

        thread_pool& operator = (thread_pool&& other) noexcept;

        void join();

        void swap(thread_pool& other) noexcept;

    private:
        std::vector<std::thread> threads;
    };
}

#endif

