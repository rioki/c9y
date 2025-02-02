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

#ifndef _C9Y_RESETTING_LATCH_H_
#define _C9Y_RESETTING_LATCH_H_

#include "defines.h"

#include <cstddef>

#include <mutex>
#include <condition_variable>

namespace c9y
{
    struct arrival_token
    {
        std::ptrdiff_t iteration;
    };

    struct noop
    {
        void operator () () {}
    };

    template <typename CompletionFunction = noop>
    class barrier
    {
    public:
        //! Returns the maximum value of the internal counter supported by the implementation.
        [[nodiscard]] static std::ptrdiff_t max() noexcept;

        explicit barrier(std::ptrdiff_t expected, CompletionFunction f = CompletionFunction())
        : count(expected), expected(expected), compl_func(f) {}

        ~barrier() = default;

        arrival_token arrive(std::ptrdiff_t n = 1) noexcept
        {
            return do_ardive(n);
        }

        void wait(arrival_token&& arrival) const
        {
            auto lock = std::unique_lock{mutex};

            if (arrival.iteration != iteration)
            {
                // previous itteration
                return;
            }

            auto this_iteration = iteration;
            cond.wait(lock, [&]{return this_iteration != iteration;});
        }

        void arrive_and_wait()
        {
            wait(arrive());
        }

        void arrive_and_drop()
        {
            do_ardive(1, 1);
        }

    private:
        std::ptrdiff_t                  count;
        std::ptrdiff_t                  iteration = 0;
        std::ptrdiff_t                  expected;
        CompletionFunction              compl_func;

        mutable std::mutex              mutex;
        mutable std::condition_variable cond;

        arrival_token do_ardive(std::ptrdiff_t n, std::ptrdiff_t drop = 0) noexcept
        {
            auto lock = std::unique_lock{mutex};

            auto at = arrival_token{iteration};

            count -= n;
            if (count > 0)
            {
                return at;
            }

            compl_func();

            expected -= drop;
            count = expected;
            iteration++;
            cond.notify_all();

            return at;
        }

        barrier(const barrier&) = delete;
        barrier& operator = (const barrier&) = delete;
        barrier(barrier&&) = delete;
        barrier& operator = (barrier&&) = delete;
    };
}

#endif
