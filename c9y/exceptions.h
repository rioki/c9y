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

#ifndef _C9Y_EXCEPTIONS_H_
#define _C9Y_EXCEPTIONS_H_

#include <functional>

#include "defines.h"

namespace c9y
{
    //! Set the unhandled exception handler.
    //!
    //! Many functions in c9y can not safely propagate excpetions without
    //! introducing data races. By default it will call std::termiante;
    //! this may not be the desired behavior. You can install a handler
    //! to customise the behavior, such as add logging and continue.
    //!
    //! @warning the handler may not throw an exception else std::termiante
    //! will be called when it hits the next noexcept.
    //!
    //! @param handler the new handler
    //! @return the previous handler
    //!
    //! @param id the thread id of the main thread.
    C9Y_EXPORT std::function<void ()> set_unhandled_exception(const std::function<void ()>& handler) noexcept;

    //! React to an unhaled exception.
    //!
    //! In a catch block at the top of your call stack, where
    //! you can't safly throw an exception, call unhandled_exception
    //! and the unhandled exception handler will be called.
    C9Y_EXPORT void unhandled_exception() noexcept;
}

#endif
