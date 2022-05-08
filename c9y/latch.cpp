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

#include "latch.h"

namespace c9y
{
    std::ptrdiff_t latch::max() noexcept
    {
        return std::numeric_limits<std::ptrdiff_t>::max();
    }

    latch::latch(std::ptrdiff_t expected)
    : count(expected) {}

    void latch::count_down(std::ptrdiff_t n)
    {
        auto lock = std::unique_lock<std::mutex>{mutex};
        count -= n;
        if (count <= 0)
        {
            cond.notify_all();
        }
    }

    bool latch::try_wait() const noexcept
    {
        wait();
        return true;
    }

    void latch::wait() const
    {
        auto lock = std::unique_lock<std::mutex>{mutex};
        if (count > 0)
        {
            cond.wait(lock);
        }
    }
}