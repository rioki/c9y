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

#ifndef _C9Y_SYNC_H_
#define _C9Y_SYNC_H_

#include <cassert>
#include <memory>
#include <thread>
#include <future>
#include <functional>

#include "defines.h"

namespace c9y
{
    //! Tag used to prevent multiple calls to the same feature.
    struct once_tag
    {
        std::atomic<bool> active = false;
    };

    //! Set the main thead id.
    //!
    //! Before using sync without the thread id, you should set the main thread id.
    //! This can be any thread, as long as the thread calls sync_point
    //! regularly.
    //!
    //! @param id the thread id of the main thread.
    C9Y_EXPORT void set_main_thread_id(const std::thread::id& id) noexcept;

    //! Get the main thread id.
    //!
    //! @returns the main thread id set by set_main_thread_id
    //!
    //! @see set_main_thread_id
    C9Y_EXPORT [[nodiscard]] std::thread::id get_main_thread_id() noexcept;

    //! Queue action to be exectued by the given thread.
    //!
    //! @param thread the thread id of the thread to call on
    //! @param func the fuction to call in the give thread
    //!
    //! @warning If any exception is thrown out of the function
    //! the sync_point will call terminate. Use sync<T> to get a
    //! future that can handle the exception.
    //!
    //! @see sync_point
    C9Y_EXPORT void sync(const std::thread::id& thread, const std::function<void ()>& func) noexcept;

    //! Queue action to be exectued by the given thread.
    //!
    //! @param tag once_tag used to prevent calls to the same function
    //! @param thread the thread id of the thread to call on
    //! @param func the fuction to call in the give thread
    //!
    //! @warning If any exception is thrown out of the function
    //! the sync_point will call terminate. Use sync<T> to get a
    //! future that can handle the exception.
    //!
    //! @see sync_point
    C9Y_EXPORT void sync(once_tag& tag, const std::thread::id& thread, const std::function<void ()>& func) noexcept;

    //! Queue action to be executed by the main thread.
    //!
    //! @func the function to call in the main thread
    //!
    //! @warning If any exception is thrown out of the function
    //! the sync_point will call terminate. Use sync<T> to get a
    //! future that can handle the exception.
    //!
    //! @see set_main_thread_id
    //! @see sync_point
    C9Y_EXPORT void sync(const std::function<void ()>& func) noexcept;

    //! Queue action to be executed by the main thread.
    //!
    //! @param tag once_tag used to prevent calls to the same function
    //! @func the function to call in the main thread
    //!
    //! @warning If any exception is thrown out of the function
    //! the sync_point will call terminate. Use sync<T> to get a
    //! future that can handle the exception.
    //!
    //! @see set_main_thread_id
    //! @see sync_point
    C9Y_EXPORT void sync(once_tag& tag, const std::function<void ()>& func) noexcept;

    //! Queue action to be exectued by the given thread with result.
    //!
    //! @param thread the thread id of the thread to call on
    //! @param func the fuction to call in the give thread
    //!
    //! @see sync_point
    template <typename T>
    [[nodiscard]] std::future<T> sync(const std::thread::id& thread, const std::function<T()>& func) noexcept
    {
        auto task = std::make_shared<std::packaged_task<T()>>(func);
        auto future = task->get_future();
        sync(thread, [task] () mutable {
            (*task)();
        });
        return future;
    }

    //! Queue action to be exectued by the given thread with result.
    //!
    //! @param tag once_tag used to prevent calls to the same function
    //! @param thread the thread id of the thread to call on
    //! @param func the fuction to call in the give thread
    //!
    //! @see sync_point
    template <typename T>
    [[nodiscard]] std::future<T> sync(once_tag& tag, const std::thread::id& thread, const std::function<T()>& func) noexcept
    {
        auto task = std::make_shared<std::packaged_task<T()>>(func);
        auto future = task->get_future();
        sync(tag, thread, [task] () mutable {
            (*task)();
        });
        return future;
    }

    //! Queue task to be executed on the main thread with result.
    //!
    //! @func the function to call in the main thread
    //!
    //! @see set_main_thread_id
    //! @see sync_point
    template <typename T>
    [[nodiscard]] std::future<T> sync(const std::function<T()>& func) noexcept
    {
        return sync<T>(get_main_thread_id(), func);
    }

    //! Queue task to be executed on the main thread with result.
    //!
    //! @param tag once_tag used to prevent calls to the same function
    //! @func the function to call in the main thread
    //!
    //! @see set_main_thread_id
    //! @see sync_point
    template <typename T>
    [[nodiscard]] std::future<T> sync(once_tag& tag, const std::function<T()>& func) noexcept
    {
        return sync<T>(tag, get_main_thread_id(), func);
    }

    //! Delay action until next time tick is called on this thread.
    //!
    //! @param func the function to call later
    //!
    //! @warning If any exception is thrown out of the function
    //! the sync_point will call terminate. Use sync<T> to get a
    //! future that can handle the exception.
    //!
    //! @see sync_point
    C9Y_EXPORT void delay(const std::function<void ()>& func) noexcept;

    //! Delay action until next time tick is called on this thread.
    //!
    //! @param tag once_tag used to prevent calls to the same function
    //! @param func the function to call later
    //!
    //! @warning If any exception is thrown out of the function
    //! the sync_point will call terminate. Use sync<T> to get a
    //! future that can handle the exception.
    //!
    //! @see sync_point
    C9Y_EXPORT void delay(once_tag& tag, const std::function<void ()>& func) noexcept;

    //! Execute alle queueed tasks for this thread.
    //!
    //! This function actually calls the tasks queued by sync and delay.
    C9Y_EXPORT void sync_point() noexcept;

    //! Syncronous Function Adapter
    //!
    //! To install callbacks that sync on the main thread.
    //!
    //! @param that the this pointer to use
    //! @param method the class method to call.
    //! @{
    [[nodiscard]] inline std::function<void ()> sync_fun(const std::function<void ()>& func) noexcept
    {
        return [func] () {
            sync(func);
        };
    }
    [[nodiscard]] inline std::function<void ()> sync_fun(const std::thread::id& thread, const std::function<void ()>& func) noexcept
    {
        return [thread, func] () {
            sync(thread, func);
        };
    }
    [[nodiscard]] inline std::function<void ()> sync_fun(once_tag& tag, const std::function<void ()>& func) noexcept
    {
        return [&tag, func] () {
            sync(tag, func);
        };
    }
    [[nodiscard]] inline std::function<void ()> sync_fun(once_tag& tag,  const std::thread::id& thread, const std::function<void ()>& func) noexcept
    {
        return [&tag, thread, func] () {
            sync(tag, thread, func);
        };
    }
    //! @}
}

#endif
