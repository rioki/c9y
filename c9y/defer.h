// c9y - concurrency
// Copyright 2017-2025 Sean Farrell
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

#ifndef _C9Y_DEFER_H_
#define _C9Y_DEFER_H_
#include "defines.h"

#include <functional>

namespace c9y
{
    //! Schedule some work to be exectued on this thread later in catch_up.
    //!
    //! This function runs lockless and thus is quite effient and does not
    //! mess with other threads.
    //!
    //! @param fun the function the schedule
    C9Y_EXPORT
    void defer(std::function<void()> fun);

    //! Exectue any pending work added by defer.
    //!
    //! @returns true if any work was executed.
    C9Y_EXPORT
    bool catch_up();
}

#endif
