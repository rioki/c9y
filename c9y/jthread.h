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

#ifndef _C9Y_JTHREAD_H_
#define _C9Y_JTHREAD_H_

#include <thread>
#include <memory>
#include <functional>

#include "defines.h"

namespace c9y
{
    #ifdef __cpp_lib_jthread
    using std::jthread;
    using std::stop_token;
    using std::stop_source;
    using std::stop_callback;
    using std::nostopstate;
    #else
    struct StopState;

    struct nostopstate_t {};
    constexpr auto nostopstate = nostopstate_t{};

    class C9Y_EXPORT stop_token
    {
    public:
        stop_token();
        explicit stop_token(std::shared_ptr<StopState> state);
        stop_token(const stop_token& other) noexcept;
        stop_token(stop_token&& other) noexcept;
        ~stop_token();

        stop_token& operator = (const stop_token& other) noexcept;
        stop_token& operator = (stop_token&& other) noexcept;

        [[nodiscard]] bool stop_requested() noexcept;
        [[nodiscard]] bool stop_possible() const noexcept;

        void swap(stop_token& other) noexcept;

    private:
        std::shared_ptr<StopState> state;

    friend class stop_callback;
    };

    inline void swap(stop_token& lhs, stop_token& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    class C9Y_EXPORT stop_source
    {
    public:
        stop_source();
        explicit stop_source(nostopstate_t nss) noexcept;
        stop_source(const stop_source& other) noexcept;
        stop_source(stop_source&& other) noexcept;
        ~stop_source();

        stop_source& operator = (const stop_source& other) noexcept;
        stop_source& operator = (stop_source&& other) noexcept;

        [[nodiscard]] stop_token get_token() const noexcept;

        bool request_stop() noexcept;
        [[nodiscard]] bool stop_possible() const noexcept;

        void swap(stop_source& other) noexcept;

    private:
        std::shared_ptr<StopState> state;
    };

    inline void swap(stop_source& lhs, stop_source& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    class C9Y_EXPORT stop_callback
    {
    public:
        template<class C>
        explicit stop_callback(const stop_token& st, C&& cb)
        : token(st), callback(cb)
        {
            self_register();
        }

        template<class C>
        explicit stop_callback(stop_token&& st, C&& cb )
        : token(std::forward<stop_token>(st)), callback(cb)
        {
            self_register();
        }

        ~stop_callback();

    private:
        stop_token             token;
        std::function<void ()> callback;

        void self_register();

        stop_callback(const stop_callback&) = delete;
        stop_callback(stop_callback&&) = delete;
        stop_callback& operator = (const stop_callback& other) noexcept = delete;
        stop_callback& operator = (stop_callback&& other) noexcept = delete;

    friend struct StopState;
    };

    //! Drop in replacement for all cases where std::jthread is not yet implemented.
    //!
    //! @see std::jthread
    class C9Y_EXPORT jthread
    {
    public:
        using id = 	std::thread::id;

        jthread() noexcept;
        jthread(jthread&& other) noexcept;

        template<class Function, class... Args>
        requires std::is_invocable_v<std::decay_t<Function>, std::decay_t<Args>...>
        explicit jthread(Function&& f, Args&&... args)
        : impl(std::forward<Function>(f), std::forward<Args>(args)...) {}

        template<class Function, class... Args>
        requires std::is_invocable_v<std::decay_t<Function>, stop_token, std::decay_t<Args>...>
        explicit jthread(Function&& f, Args&&... args)
        : impl(std::forward<Function>(f), stop.get_token(), std::forward<Args>(args)...) {}

        ~jthread();

        jthread& operator = (jthread&& other) noexcept;

        [[nodiscard]] id get_id() const noexcept;
        [[nodiscard]] bool joinable() const noexcept;

        void join();
        void detach();

        [[nodiscard]] stop_source get_stop_source() noexcept;
        [[nodiscard]] stop_token get_stop_token() noexcept;
        bool request_stop() noexcept;

        void swap(jthread& other) noexcept;

    private:
        stop_source stop = {};
        std::thread impl;

        jthread(const jthread&) = delete;
        jthread& operator = (const jthread& other) noexcept = delete;
    };

    template<class Function, class... Args>
    requires std::is_invocable_v<std::decay_t<Function>, std::decay_t<Args>...>
    std::thread make_thread(Function&& f, stop_token token, Args&&... args)
    {
        return std::thread{std::forward<Function>(f), std::forward<Args>(args)...};
    }

    template<class Function, class... Args>
    requires std::is_invocable_v<std::decay_t<Function>, stop_token, std::decay_t<Args>...>
    std::thread make_thread(Function&& f, stop_token token, Args&&... args)
    {
        return std::thread{std::forward<Function>(f), token, std::forward<Args>(args)...};
    }

    inline void swap(jthread& lhs, jthread& rhs) noexcept
    {
        lhs.swap(rhs);
    }
    #endif
}

#endif
