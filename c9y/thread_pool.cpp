//
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
//

#include "thread_pool.h"

#include <algorithm>
#include <ranges>

namespace c9y
{
    #ifndef __cpp_lib_jthread
    thread_pool::~thread_pool()
    {
        join();
    }
    #endif

    size_t thread_pool::get_concurency() const
    {
        return threads.size();
    }

    void thread_pool::join()
    {
        for (auto& thread : threads)
        {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    #ifdef __cpp_lib_jthread
    bool thread_pool::request_stop() noexcept
    {
        return std::ranges::all_of(threads, [] (auto& thread) {
            return thread.request_stop();
        });
    }
    #endif
}
