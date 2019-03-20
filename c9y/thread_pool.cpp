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

#include "thread_pool.h"

namespace c9y
{
    thread_pool::thread_pool() noexcept {}

    thread_pool::thread_pool(std::function<void()> thread_func, size_t concurency)
    {
        for (size_t i = 0; i < concurency; i++)
        {
            threads.emplace_back(std::thread(thread_func));
        }
    }

    thread_pool::thread_pool(thread_pool&& other) noexcept
    {
        swap(other);
    }

    thread_pool::~thread_pool() {}

    thread_pool& thread_pool::operator = (thread_pool&& other) noexcept
    {
        swap(other);
        return *this;
    }

    size_t thread_pool::get_concurency() const
    {
        return threads.size();
    }

    void thread_pool::join()
    {
        for (auto& thread : threads)
        {
            thread.join();
        }
        threads.clear();
    }

    void thread_pool::swap(thread_pool& other) noexcept
    {
        threads.swap(other.threads);
    }
}
