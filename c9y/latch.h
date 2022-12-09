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

#ifndef _C9Y_LATCH_H_
#define _C9Y_LATCH_H_

#include "defines.h"

#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <chrono>

#ifdef max
#warning "Undefined max. Defining the max macro will result in a compile error."
#undef max
#endif

namespace c9y
{
    //! latch
    //!
    //! The latch class is a downward counter of type ptrdiff_t which can be used to synchronize threads.
    //! The value of the counter is initialized on creation. Threads may block on the latch until the counter
    //! is decremented to zero. There is no possibility to increase or reset the counter, which makes the
    //! latch a single-use barrier.
    //! Concurrent invocations of the member functions of latch, except for the destructor, do not introduce data races.
    class C9Y_EXPORT latch
    {
    public:
        //! Returns the maximum value of the internal counter supported by the implementation.
        static [[nediscard]] std::ptrdiff_t max() noexcept;

        //! Constructs a latch and initializes its internal counter. The behavior is undefined if expected is negative or greater than max().
        explicit latch(std::ptrdiff_t expected);

        ~latch() = default;

        //! Atomically decrements the internal counter by n without blocking the caller.
        //!
        //! If n is greater than the value of the internal counter or is negative, the behavior is undefined.
        //! This operation strongly happens-before all calls that are unblocked on this latch.
        void count_down(std::ptrdiff_t n = 1);

        //! Returns true only if the internal counter has reached zero.
        //!
        //! This function may spuriously return false with very low probability even if the internal counter has reached zero.
        template<class Rep, class Period>
        [[nediscard]] bool wait_for(const std::chrono::duration<Rep, Period>& duration) const noexcept
        {
            auto lock = std::unique_lock<std::mutex>{mutex};
            cond.wait_for(lock, duration, [&]{return count == 0;});
            return count == 0;
        }

        //! Blocks the calling thread until the internal counter reaches 0. If it is zero already, returns immediately.
        void wait() const;

    private:
        std::ptrdiff_t                  count;
        mutable std::mutex              mutex;
        mutable std::condition_variable cond;

        latch(const latch&) = delete;
        latch& operator = (const latch&) = delete;
    };
}

#endif
