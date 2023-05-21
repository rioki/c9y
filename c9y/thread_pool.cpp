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

#include "thread_pool.h"

#include <algorithm>
#ifdef __cpp_lib_jthread
#include <ranges>
#endif

namespace c9y
{
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

    bool thread_pool::request_stop() noexcept
    {
        auto result = false;
        for (auto& thread : threads)
        {
            // the result for one must be the same as for all; unless
            // multuple threads call request_stop at the same time;
            // but then we are in a "tree in the wood, nobody listens" situation.
            result = thread.request_stop();
        }
        return result;
    }
}
