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
    //! resetting latch
    //!
    //! The resetting latch class is a downward counter of type ptrdiff_t which can be used to synchronize threads.
    //! The value of the counter is initialized on creation. Threads may block on the latch until the counter
    //! is decremented to zero. In contrast to latch, the resetting_latch will once the count is reached and
    //! reset.
    class C9Y_EXPORT resetting_latch
    {
    public:

        //! Constructs a latch and initializes its internal counter. The behavior is undefined if expected is negative or greater than max().
        explicit resetting_latch(std::ptrdiff_t expected) noexcept;

        ~resetting_latch() = default;

        //! Atomically decrements the internal counter by n without blocking the caller.
        //!
        //! If n is greater than the value of the internal counter or is negative, the behavior is undefined.
        //! This operation strongly happens-before all calls that are unblocked on this latch.
        void count_down(std::ptrdiff_t n = 1) noexcept;

        //! Blocks the calling thread until the internal counter reaches 0.
        //!
        //! @warning There is a subtle race condition between count_down and wait,
        //! that latch does not have. You need to make sure your condition
        //! you are waiting on is not yet achived. If that is the case, don't wait
        //! since the latch is now in it's next iteration
        void wait() const;

        //! decrements the counter and blocks until it reaches zero
        //!
        //! Atomically decrements the internal counter by n and (if necessary) blocks the
        //! calling thread until the counter reaches zero. After the release of all threads,
        //! the counter is reset.
        void arrive_and_wait(std::ptrdiff_t n = 1);

    private:
        std::ptrdiff_t                  count;
        std::ptrdiff_t                  iteration = 0;
        std::ptrdiff_t                  expected;

        mutable std::mutex              mutex;
        mutable std::condition_variable cond;

        bool do_count_down(std::unique_lock<std::mutex>& lock, std::ptrdiff_t n = 1) noexcept;
        void do_wait(std::unique_lock<std::mutex>& lock) const noexcept;

        resetting_latch(const resetting_latch&) = delete;
        resetting_latch& operator = (const resetting_latch&) = delete;
        resetting_latch(resetting_latch&&) = delete;
        resetting_latch& operator = (resetting_latch&&) = delete;
    };
}

#endif
