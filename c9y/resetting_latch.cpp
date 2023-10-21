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


#include "resetting_latch.h"

#include <cassert>

namespace c9y
{
    resetting_latch::resetting_latch(std::ptrdiff_t _expected) noexcept
    : count(_expected), expected(_expected) {}

    void resetting_latch::count_down(std::ptrdiff_t n) noexcept
    {
        auto lock = std::unique_lock{mutex};
        do_count_down(lock);
    }

    void resetting_latch::wait() const
    {
        auto lock = std::unique_lock<std::mutex>{mutex};
        do_wait(lock);
    }

    void resetting_latch::arrive_and_wait(std::ptrdiff_t n)
    {
        auto lock = std::unique_lock{mutex};
        if (do_count_down(lock, n))
        {
            do_wait(lock);
        }
    }

    bool resetting_latch::do_count_down(std::unique_lock<std::mutex>& /*lock*/, std::ptrdiff_t n) noexcept
    {
        count -= n;
        if (count > 0)
        {
            return true;
        }

        count = expected;
        iteration++;
        cond.notify_all();
        return false;
    }

    void resetting_latch::do_wait(std::unique_lock<std::mutex>& lock) const noexcept
    {
        auto this_itteration = iteration;
        cond.wait(lock, [&]{return this_itteration != iteration;});
    }
}
