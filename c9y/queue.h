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

#ifndef _C9Y_QUEUE_H_
#define _C9Y_QUEUE_H_

#include <mutex>
#include <condition_variable>
#include <deque>
#include <chrono>
#include <optional>

namespace c9y
{
    using namespace std::chrono_literals;

    //! Thread Safe Queue
    //!
    //! This is a thread safe implementation of a queue.
    //!
    //! @note There is no really safe way to copy a queue, so this
    //! queue is not copyable or asingable.
    template <typename T, class Container = std::deque<T>>
    class queue
    {
    public:
        using container_type  =  Container;
        using value_type      =  typename Container::value_type;
        using size_type       =  typename Container::size_type;
        using reference       =  typename Container::reference;
        using const_reference =  typename Container::const_reference;

        //! Create an empty queue.
        queue() noexcept(std::is_nothrow_constructible_v<Container>) = default;

        //! Initialize the queue with a reange of values.
        //!
        //! @param begin an iterator to the beginning of the range
        //! @param end an iterator to the one beond the end of the range
        template<typename... Args>
        explicit queue(Args&&... args) noexcept(std::is_nothrow_constructible_v<Container, Args...>)
        : container(std::forward<Args>(args)...) {}

        //! Destructor
        ~queue() = default;

        //! Push a value onto the queue.
        //!
        //! This method will push the value onto the queue and
        //! wake up a thread that is wating in pop_wait.
        //!
        //! @param value the value to push onto the queue
        //!
        //! @{
        void push(const value_type& value) noexcept(std::is_nothrow_copy_constructible_v<value_type>)
        {
            {
                auto lock = std::unique_lock<std::mutex>{mutex};
                container.push_back(value);
            }
            cond.notify_one();
        }

        template<typename... Args>
        void emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>)
        {
            {
                auto lock = std::unique_lock<std::mutex>{mutex};
                container.emplace_back(std::forward<Args>(args)...);
            }
            cond.notify_one();
        }
        //! @}

        //! Pop a value of the queue.
        //!
        //! This method will try to pop a value off the queue. If no value is
        //! in the queue, it will return false.
        //!
        //! @param value the value of the pop
        //! @return true if a value was poped of the queue
        [[nodiscard]] std::optional<value_type> pop() noexcept
        {
            auto lock = std::unique_lock<std::mutex>{mutex};
            if (!container.empty())
            {
                auto value = container.front();
                container.pop_front();
                return value;
            }
            else
            {
                return std::nullopt;
            }
        }

        //! Pop a value of the queue, wait if nessesary.
        //!
        //! This method will try to pop a value off the queue. If no value is
        //! in the queue, it will wait until either a value is pushed onto the
        //! queue or wake is called.
        //!
        //! @param value the value of the pop
        //! @return true if a value was poped of the queue
        //!
        //! @warning It is quite simple to build a race condition with pop_wait
        //! and wake. If you intend to reliably wake all waiting threads, use
        //! pop_wait_for with a reasonable timeout.
        [[nodiscard]] std::optional<value_type> pop_wait() noexcept
        {
            auto lock = std::unique_lock<std::mutex>{mutex};
            cond.wait(lock, [&]{return !container.empty() || stopped;});

            if (container.empty())
            {
                return std::nullopt;
            }

            auto value = container.front();
            container.pop_front();
            return value;
        }

        //! Pop a value of the queue, wait for a defined duration if nessesary.
        //!
        //! This method will try to pop a value off the queue. If no value is
        //! in the queue, it will wait until either a value is pushed onto the
        //! queue or wake is called.
        //!
        //! @param value the value of the pop
        //! @param duration the duration to wait for
        //! @return true if a value was poped of the queue
        template<class Rep, class Period>
        [[nodiscard]] std::optional<value_type> pop_wait_for(const std::chrono::duration<Rep, Period>& duration) noexcept
        {
            auto lock = std::unique_lock<std::mutex>{mutex};
            cond.wait_for(lock, duration, [&]{return !container.empty() || stopped;});

            if (container.empty())
            {
                return std::nullopt;
            }

            auto value = container.front();
            container.pop_front();
            return value;
        }

        //! Stop processing and wake any wating threads.
        //!
        //! This function should be called before destructing the queue and ensure
        //! a clean exit can be done.
        void stop() noexcept
        {
            {
                auto lock = std::unique_lock<std::mutex>{mutex};
                stopped = true;
            }
            cond.notify_all();
        }

    private:
        std::mutex              mutex;
        std::condition_variable cond;
        Container               container;
        bool                    stopped = false;

        queue(const queue& other) = delete;
        queue& operator = (const queue& other) = delete;
    };
}

#endif
