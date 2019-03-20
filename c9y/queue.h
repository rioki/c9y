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

#ifndef _C9Y_QUEUE_H_
#define _C9Y_QUEUE_H_

#include <mutex>
#include <condition_variable>
#include <deque>
#include <chrono>

namespace c9y
{
    using namespace std::chrono_literals;

    /**
     * Thread Safe Queue
     *
     * This is a thread safe implementation of a queue.
     *
     * @note There is no really safe way to copy a queue, so this
     * queue is not copyable or asingable.
     **/
    template <typename T, class Container = std::deque<T>>
    class queue
    {
    public:
        typedef Container                           container_type;
        typedef typename Container::value_type      value_type;
        typedef typename Container::size_type       size_type;
        typedef typename Container::reference       reference;
        typedef typename Container::const_reference const_reference;

        /**
         * Create an empty queue.
         **/
        queue() {}

        /**
         * Initialize the queue with values.
         *
         * @param c the container to initialize the queue with
         **/
        explicit
        queue(const Container& c)
        : container(c) {}

        /**
         * Initialize the queue with a reange of values.
         *
         * @param begin an iterator to the beginning of the range
         * @param end an iterator to the one beond the end of the range
         **/
        template <class Iterator>
        queue(const Iterator& begin, const Iterator& end)
        : container(begin, end) {}

        queue(const queue& other) = delete;

        /**
         * Destructor
         **/
        ~queue()
        {
            wake();
        }

        queue& operator = (const queue& other) = delete;

        /**
         * Push a value onto the queue.
         *
         * @param value the value to push onto the queue
         *
         * This method will push the value onto the queue and
         * wake up a thread that is wating in pop_wait.
         **/
        void push(const value_type& value)
        {
            auto lock = std::unique_lock<std::mutex>{mutex};
            container.push_back(value);
            cond.notify_one();
        }

        /**
         * Pop a value of the queue.
         *
         * @param value the value of the pop
         * @return true if a value was poped of the queue
         *
         * This method will try to pop a value off the queue. If no value is
         * in the queue, it will return false.
         **/
        bool pop(value_type& value)
        {
            auto lock = std::unique_lock<std::mutex>{mutex};
            if (!container.empty())
            {
                value = container.front();
                container.pop_front();
                return true;
            }
            else
            {
                return false;
            }
        }

        /**
         * Pop a value of the queue, wait if nessesary.
         *
         * @param value the value of the pop
         * @return true if a value was poped of the queue
         *
         * This method will try to pop a value off the queue. If no value is
         * in the queue, it will wait until either a value is pushed onto the
         * queue or wake is called.
         **/
        bool pop_wait(value_type& value)
        {
            auto lock = std::unique_lock<std::mutex>{mutex};

            if (container.empty())
            {
                cond.wait(lock);
            }

            if (!container.empty())
            {
                value = container.front();
                container.pop_front();
                return true;
            }
            else
            {
                return false;
            }
        }

        /**
         * Pop a value of the queue, wait for a defined duration if nessesary.
         *
         * @param value the value of the pop
         * @param duration the duration to wait for
         * @return true if a value was poped of the queue
         *
         * This method will try to pop a value off the queue. If no value is
         * in the queue, it will wait until either a value is pushed onto the
         * queue or wake is called.
         **/
        bool pop_wait_for(value_type& value, std::chrono::milliseconds duration)
        {
            auto lock = std::unique_lock<std::mutex>{mutex};

            if (container.empty())
            {
                cond.wait_for(lock, duration);
            }

            if (!container.empty())
            {
                value = container.front();
                container.pop_front();
                return true;
            }
            else
            {
                return false;
            }
        }

        /**
         * Wake up any threads that are wating in pop_wait.
         **/
        void wake()
        {
            cond.notify_all();
        }

    private:
        std::mutex              mutex;
        std::condition_variable cond;
        Container               container;
    };
}

#endif
