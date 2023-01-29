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

#include "jthread.h"

#ifndef C9Y_USE_STD_JTHREAD
#include <cassert>
#include <atomic>
#include <mutex>
#include <vector>

namespace c9y
{
    // this is not efficient, but it only needs to be correct
    class stop_state
    {
    public:
        stop_state() = default;
        ~stop_state() = default;

        bool request_stop() noexcept
        {
            auto lk = std::unique_lock<std::mutex>(mutex);
            if (stopping == true)
            {
                return false;
            }

            stopping = true;
            for (const auto& cb : callbacks)
            {
                assert(cb->callback);
                cb->callback();
            }
            callbacks.clear();
            return true;
        }

        bool stop_requested() const noexcept
        {
            auto lk = std::unique_lock<std::mutex>(mutex);
            return stopping;
        }

        void add_callback(stop_callback* callback)
        {
            auto lk = std::unique_lock<std::mutex>(mutex);
            if (!stopping)
            {
                callbacks.push_back(callback);
            }
            else
            {
                callback->callback();
            }
        }

        void remove_callback(stop_callback* callback)
        {
            auto lk = std::unique_lock<std::mutex>(mutex);
            auto i = std::find(begin(callbacks), end(callbacks), callback);
            if (i != end(callbacks))
            {
                callbacks.erase(i);
            }
        }

    private:
        mutable std::mutex mutex;
        bool stopping = false;
        std::vector<stop_callback*> callbacks;

        stop_state(const stop_state&) = delete;
        stop_state& operator = (const stop_state&) = delete;
    };

    stop_token::stop_token() = default;

    stop_token::stop_token(std::shared_ptr<stop_state> s)
    : state(s) {}

    stop_token::stop_token(const stop_token& other) noexcept = default;
    stop_token::stop_token(stop_token&& other) noexcept = default;
    stop_token::~stop_token() = default;

    stop_token& stop_token::operator = (const stop_token& other) noexcept = default;
    stop_token& stop_token::operator = (stop_token&& other) noexcept = default;

    bool stop_token::stop_requested() noexcept
    {
        if (state)
        {
            return state->stop_requested();
        }
        return false;
    }

    bool stop_token::stop_possible() const noexcept
    {
        return static_cast<bool>(state);
    }

    void stop_token::swap(stop_token& other) noexcept
    {
        state.swap(other.state);
    }

    stop_source::stop_source()
    : state(std::make_shared<stop_state>()) {}

    stop_source::stop_source(nostopstate_t) noexcept {}

    stop_source::stop_source(const stop_source& other) noexcept = default;
    stop_source::stop_source(stop_source&& other) noexcept = default;
    stop_source::~stop_source() = default;

    stop_source& stop_source::operator = (const stop_source& other) noexcept = default;
    stop_source& stop_source::operator = (stop_source&& other) noexcept = default;

    stop_token stop_source::get_token() const noexcept
    {
        return stop_token{state};
    }

    bool stop_source::request_stop() noexcept
    {
        if (state)
        {
            return state->request_stop();
        }
        return false;
    }

    bool stop_source::stop_possible() const noexcept
    {
        return static_cast<bool>(state);
    }

    void stop_source::swap(stop_source& other) noexcept
    {
        state.swap(other.state);
    }

    stop_callback::~stop_callback()
    {
        if (token.state)
        {
            token.state->remove_callback(this);
        }
    }

    void stop_callback::self_register()
    {
        if (token.state)
        {
            token.state->add_callback(this);
        }
        else
        {
            callback();
        }
    }

    jthread::jthread() noexcept
    : stop{nostopstate} {}

    jthread::jthread(jthread&& other) noexcept = default;

    jthread::~jthread()
    {
        if (joinable())
        {
            request_stop();
            join();
        }
    }

    jthread& jthread::operator = (jthread&& other) noexcept = default;

    jthread::id jthread::get_id() const noexcept
    {
        return impl.get_id();
    }

    bool jthread::joinable() const noexcept
    {
        return impl.joinable();
    }

    void jthread::join()
    {
        impl.join();
    }

    void jthread::detach()
    {
        impl.detach();
    }

    stop_source jthread::get_stop_source() noexcept
    {
        if (joinable())
        {
            return stop;
        }
        else
        {
            return stop_source{nostopstate};
        }
    }

    stop_token jthread::get_stop_token() noexcept
    {
        return get_stop_source().get_token();
    }

    bool jthread::request_stop() noexcept
    {
        return stop.request_stop();
    }

    void jthread::swap(jthread& other) noexcept
    {
        stop.swap(other.stop);
        impl.swap(other.impl);
    }
}
#endif

