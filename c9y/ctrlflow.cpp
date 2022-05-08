//
// c9y - concurrency
// Copyright(c) 2017-2019 Sean Farrell
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
//

#include "ctrlflow.h"

#include <cassert>

#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/time.h>
#endif

namespace c9y
{
    void async(task_pool& tp, std::function<void()> task, std::function<void(std::exception_ptr err)> callback)
    {
        tp.async([&tp, task, callback]() {
            auto err = std::exception_ptr{};
            try
            {
                task();
            }
            catch (...)
            {
                err = std::current_exception();
            }
            tp.sync(std::bind(callback, err));
        });
    }

    struct idle_handle : public base_handle
    {
        std::atomic<bool>      running = true;
        std::function<bool ()> task;

        idle_handle(std::function<bool ()> t)
        : task(t) {}
    };

    void do_idle(task_pool& tp, std::shared_ptr<idle_handle> handle)
    {
        assert(handle);

        if (handle->running)
        {
            assert(handle->task);
            bool r = handle->task();
            if (r)
            {
                tp.sync(std::bind(do_idle, std::ref(tp), handle));
            }
        }
    }

    C9Y_EXPORT
    handle start_idle(task_pool& tp, std::function<bool()> task)
    {
        if (!task)
        {
            throw std::invalid_argument(__FUNCTION__);
        }

        auto h = std::make_shared<idle_handle>(task);
        tp.sync(std::bind(do_idle, std::ref(tp), h));

        return h;
    }

    C9Y_EXPORT
    void stop_idle(handle h)
    {
        auto hnd = std::dynamic_pointer_cast<idle_handle>(h);
        if (!hnd)
        {
            throw std::invalid_argument(__FUNCTION__);
        }
        hnd->running = false;
    }

    unsigned int get_ms_time()
    {
        #ifdef WINDOWS
        return timeGetTime();
        #else
        struct timeval now;
        gettimeofday(&now, NULL);
        return now.tv_usec/1000;
        #endif
    }

    struct timer_info : public base_handle
    {
        std::atomic<bool>         running = true;
        std::function<bool()>     task;
        std::atomic<unsigned int> intervall;
        std::atomic<unsigned int> last;

        timer_info(std::function<bool ()> t, unsigned int i)
        : task(t), intervall(i), last(get_ms_time()) {}
    };

    void do_timer(task_pool& tp, std::shared_ptr<timer_info> handle)
    {
        assert(handle);

        if (handle->running)
        {
            assert(handle->task);
            auto now = get_ms_time();
            auto next = handle->last + handle->intervall;

            if (now >= next)
            {
                handle->last    = get_ms_time();
                handle->running = handle->task();
            }

            if (handle->running)
            {
                tp.sync(std::bind(do_timer, std::ref(tp), handle));
            }
        }
    }

    handle start_timer(task_pool& tp, std::function<bool()> task, unsigned int intervall)
    {
        if (!task)
        {
            throw std::invalid_argument(__FUNCTION__);
        }

        auto h = std::make_shared<timer_info>(task, intervall);
        tp.sync(std::bind(do_timer, std::ref(tp), h));

        return h;
    }

    void set_intervall(handle h, unsigned int intervall)
    {
        auto hnd = std::dynamic_pointer_cast<timer_info>(h);
        if (!hnd)
        {
            throw std::invalid_argument(__FUNCTION__);
        }
        hnd->intervall = intervall;
    }

    void stop_timer(handle h)
    {
        auto hnd = std::dynamic_pointer_cast<timer_info>(h);
        if (!hnd)
        {
            throw std::invalid_argument(__FUNCTION__);
        }
        hnd->running = false;
    }

    void throw_on_error(std::exception_ptr err)
    {
        if (err)
        {
            std::rethrow_exception(err);
        }
    }
}
