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

#ifndef _C9Y_COROUTINE_H_
#define _C9Y_COROUTINE_H_

#include "defines.h"

#include <cstddef>

#if defined __cpp_lib_coroutine && defined C9Y_OLD_COROUTINE

#include <future>
#include <coroutine>

#include "async.h"
#include "sync.h"

namespace std
{
    template <typename T, typename ... Args>
    struct coroutine_traits<future<T>, Args...>
    {
        struct promise_type : promise<T>
        {
            future<T> get_return_object() noexcept
            {
                return this->get_future();
            }

            suspend_never initial_suspend() const noexcept
            {
                return {};
            }

            suspend_never final_suspend() const noexcept
            {
                return {};
            }

            void return_value(const T& value) noexcept
            {
                this->set_value(value);
            }

            void return_value(T&& value) noexcept
            {
                this->set_value(move(value));
            }

            void unhandled_exception() noexcept
            {
                this->set_exception(current_exception());
            }
        };
    };

    template <typename... Args>
    struct std::coroutine_traits<std::future<void>, Args...>
    {
        struct promise_type : promise<void>
        {
            std::future<void> get_return_object() noexcept
            {
                return this->get_future();
            }

            std::suspend_never initial_suspend() const noexcept
            {
                return {};
            }

            std::suspend_never final_suspend() const noexcept { return {}; }

            void return_void() noexcept {
                this->set_value();
            }
            void unhandled_exception() noexcept {
                this->set_exception(current_exception());
            }
        };
    };
}

namespace c9y
{
    namespace co_async
    {
        //! co_await operator implemented with async.
        template <typename T>
        auto operator co_await(std::future<T> future) noexcept
        {
            struct awaiter : std::future<T>
            {
                bool await_ready() const noexcept
                {
                    using namespace std::chrono_literals;
                    return this->wait_for(0s) != std::future_status::timeout;
                }

                void await_suspend(std::coroutine_handle<> cont) const
                {
                    c9y::async([this, cont] {
                        this->wait();
                        cont();
                    });
                }

                T await_resume()
                {
                    return this->get();
                }
            };

            return awaiter{ std::move(future) };
        }
    }

    namespace co_sync
    {
        //! co_await operator implemented with sync.
        template <typename T>
        auto operator co_await(std::future<T> future) noexcept
        {
            struct awaiter : std::future<T>
            {
                bool await_ready() const noexcept
                {
                    using namespace std::chrono_literals;
                    return this->wait_for(0s) != std::future_status::timeout;
                }

                void await_suspend(std::coroutine_handle<> cont) const
                {
                    c9y::delay([this, cont] {
                        this->wait();
                        cont();
                    });
                }

                T await_resume()
                {
                    return this->get();
                }
            };

            return awaiter{ std::move(future) };
        }
    }
}

#endif
#endif
