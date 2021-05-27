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

#ifndef _C9Y_PARALELL_H_
#define _C9Y_PARALELL_H_

#include "defines.h"

#include <functional>
#include <vector>

#include "latch.h"
#include "task_pool.h"

namespace c9y
{
    C9Y_EXPORT task_pool& _get_paralell_pool() noexcept;

    template <typename IteratorT>
    void paralell(IteratorT begin, IteratorT end) noexcept
    {
        auto& pool = _get_paralell_pool();
        latch l(std::distance(begin, end));
        for (auto i = begin; i != end; ++i)
        {
            auto task = *i;
            pool.enqueue([&, task] () {
                task();
                l.count_down();
            });
        }
        l.wait();
    }

    inline void paralell(const std::vector<std::function<void ()>>& tasks) noexcept
    {
        paralell(begin(tasks), end(tasks));
    }
}

#endif
