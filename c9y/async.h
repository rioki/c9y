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

#ifndef _C9Y_ASYNC_H_
#define _C9Y_ASYNC_H_

#include <future>
#include <functional>

#include "defines.h"

namespace c9y
{
    //! Queue action to be executed on the shared thread pool.
    //!
    //! @param func the function to execute.
    C9Y_EXPORT void async(const std::function<void ()>& func) noexcept;

    //! Queue action to be executed on the shared thread pool with result
    //!
    //! @param func the function to execute.
    //! @returns future that with the resulting value.
    template <typename T>
    std::future<T> async(const std::function<T ()>& func) noexcept
    {
        auto task = std::make_shared<std::packaged_task<T()>>(func);
        auto future = task->get_future();
        async([task] () mutable {
            (*task)();
        });
        return future;
    }
}

#endif
