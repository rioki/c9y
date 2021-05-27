//
// c9y - concurrency
// Copyright(c) 2017-2021 Sean Farrell
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

#include "sync.h"

#include <stdexcept>
#include <mutex>
#include <map>
#include <iostream>

namespace c9y
{
    std::mutex tasks_mutex;
    std::map<std::thread::id, std::vector<std::function<void ()>>> tasks;
    std::atomic<std::thread::id> main_thread_id;

    void set_main_thread_id(const std::thread::id& id) noexcept
    {
        main_thread_id = id;
    }

    std::thread::id get_main_thread_id() noexcept
    {
        return main_thread_id;
    }

    void delay(const std::function<void()>& func) noexcept
    {
        sync(std::this_thread::get_id(), func);
    }

    void sync(const std::function<void()>& func) noexcept
    {
        assert(main_thread_id != std::thread::id());
        sync(main_thread_id, func);
    }

    void sync(const std::thread::id& thread, const std::function<void()>& func) noexcept
    {
        assert(func);
        std::scoped_lock<std::mutex> sl(tasks_mutex);
        tasks[thread].push_back(func);
    }

    std::vector<std::function<void()>> get_this_threads_tasks() noexcept
    {
        std::scoped_lock<std::mutex> sl(tasks_mutex);
        auto this_threads_tasks = tasks[std::this_thread::get_id()];
        tasks[std::this_thread::get_id()] = std::vector<std::function<void ()>>();
        return this_threads_tasks;
    }

    void sync_point() noexcept
    {
        auto tasks = get_this_threads_tasks();
        for (const auto& task : tasks)
        {
            try
            {
                task();
            }
            catch (const std::exception& ex)
            {
                std::cerr << ex.what() << std::endl;
                std::terminate();
            }
            catch (...)
            {
                std::terminate();
            }
        }
    }
}