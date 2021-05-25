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

#ifndef _C9Y_ALGORITHM_H_
#define _C9Y_ALGORITHM_H_

#include <atomic>
#include <list>
#include <future>
#include <map>

#include "defines.h"
#include "task_pool.h"

namespace c9y
{
    /**
     * The default chunking size.
     *
     * The STL like algorithms are generally executed with very small
     * tasks and this may be inefficient to queue each one individually.
     * As a result they are chunked together. This is the default chunk size
     * but each has it's own configurable value.
     */
    constexpr auto default_chunk_size = 32u;

    struct _paralell_state
    {
        std::atomic<unsigned int> count;
        std::atomic<bool> has_err = false;
        std::exception_ptr err;

        _paralell_state(unsigned int c)
        : count(c) {}
    };

    /**
     * Execute a sequence of tasks in parallel.
     *
     * This function executes a sequence of tasks on the asynchronous task stream
     * and once done execute the callback on the sync stream. This function can
     * be used to batch related work.
     *
     * @param tp the task pool to execute on
     * @param begin the beginning of the sequence
     * @param end the end of the sequence
     * @param callback the callback called when all tasks are executed
     *
     * @note If multiple tasks throw exceptions it is not defined which
     * exception will be reported to the callback. The task execution
     * is not interrupted by an exception, but there is no guarantee if
     * everything was exectured.
     *
     * @see async
     **/
    template <class Iterator>
    void paralell(task_pool& tp, const Iterator& begin, const Iterator& end, std::function<void(std::exception_ptr err)> callback)
    {
        auto shared_state = std::make_shared<_paralell_state>(static_cast<unsigned int>(std::distance(begin, end)));

        auto i = begin;
        while (i != end)
        {
            auto task = *i;
            tp.async([&tp, task, shared_state, callback] () {
                try
                {
                    task();
                }
                catch (...)
                {
                    if (shared_state->has_err.exchange(true) == false)
                    {
                        shared_state->err = std::current_exception();
                    }
                }

                if (--(shared_state->count) == 0)
                {
                    tp.sync([callback, shared_state] () {
                        callback(shared_state->err);
                    });
                }
            });
            ++i;
        }
    }

    /**
     * Execute a collection of tasks in parallel.
     *
     * This function is a convenience wrapper around parallel, that takes
     * an entire collection instead of two iterators.
     *
     * @param tp the task pool to execute on
     * @param collection the collection of tasks
     * @param callback the callback called when all tasks are executed
     *
     * @see async
     **/
    template <class Collection>
    void paralell(task_pool& tp, const Collection& collection, std::function<void(std::exception_ptr err)> callback)
    {
        paralell(tp, begin(collection), end(collection), callback);
    }

    C9Y_EXPORT
    void _sequence_one(task_pool& tp, std::shared_ptr<std::list<std::function<void ()>>> shared_tasks, std::function<void(std::exception_ptr err)> callback);

    /**
     * Execute a sequence of tasks in sequence.
     *
     * This function executes a sequence of tasks in sequence on the asynchronous
     * task stream and once done execute the callback on the sync stream.
     * This function can be used to batch related work that has a strict ordering.
     *
     * @param tp the task pool to execute on
     * @param begin the beginning of the sequence
     * @param end the end of the sequence
     * @param callback the callback called when all tasks are executed
     *
     * @note If a task throws an exception, the exception will interrupt the
     * sequence execution.
     *
     * @see async
     **/
    template <class Iterator>
    void sequence(task_pool& tp, const Iterator& begin, const Iterator& end, std::function<void(std::exception_ptr err)> callback)
    {
        auto shared_tasks = std::make_shared<std::list<std::function<void()>>>(begin, end);
        tp.async([&tp, shared_tasks, callback] () {
            _sequence_one(tp, shared_tasks, callback);
        });
    }

    /**
     * Execute a collection of tasks in sequence.
     *
     * This function executes a collection of tasks in sequence on the asynchronous
     * task stream and once done execute the callback on the sync stream.
     * This function can be used to batch related work that has a strict ordering.
     *
     * @param tp the task pool to execute on
     * @param collection the collection of tasks
     * @param callback the callback called when all tasks are executed
     *
     * @note If a task throws an exception, the exception will interrupt the
     * sequence execution.
     *
     * @see async
     **/
    template <class Collection>
    void sequence(task_pool& tp, const Collection& collection, std::function<void(std::exception_ptr err)> callback)
    {
        sequence(tp, begin(collection), end(collection), callback);
    }

    template <class Iterator>
    size_t safe_advance(Iterator& iter, const Iterator& end, size_t n)
    {
        auto remaining = std::distance(iter, end);
        if (static_cast<size_t>(remaining) < n)
        {
            n = remaining;
        }
        std::advance(iter, n);
        return n;
    }

    /**
     * Generate a number of values.
     *
     * This function emulates std::generate, but runs in parallel on the asynchronous task stream.
     *
     * @param tp the task pool to execute on
     * @param start beginning of the sequence
     * @param end the end of the sequence
     * @param generator the function to generate values
     * @param callback the callback called when all tasks are executed
     * @param chunk_size the size of the batches used to form tasks
     *
     * @warning The iterators used must be valid until callback is called.
     *
     * @see parallel
     **/
    template <class Iterator, class Generator>
    void generate(task_pool& tp, Iterator start, Iterator end, Generator generator, std::function<void(std::exception_ptr err)> callback, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks = std::list<std::function<void ()>>{};

        auto b = start;
        auto e = b;
        safe_advance(e, end, chunk_size);
        while (b != end)
        {
            tasks.push_back([b, e, generator] () {
                std::generate(b, e, generator);
            });
            b = e;
            safe_advance(e, end, chunk_size);
        }

        paralell(tp, tasks, callback);
    }

    /**
     * Call a function for each element in a collection..
     *
     * This function emulates std::function, but runs in parallel on the asynchronous task stream.
     *
     * @param tp the task pool to execute on
     * @param start beginning of the sequence
     * @param end the end of the sequence
     * @param generator the function to generate values
     * @param callback the callback called when all tasks are executed
     * @param chunk_size the size of the batches used to form tasks
     *
     * @warning The iterators used must be valid until callback is called.
     *
     * @see parallel
     **/
    template <class Iterator, class UnaryFunction>
    void for_each(task_pool& tp, Iterator start, Iterator end, UnaryFunction function, std::function<void(std::exception_ptr err)> callback, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks = std::list<std::function<void ()>>{};

        auto b = start;
        auto e = b;
        safe_advance(e, end, chunk_size);
        while (b != end)
        {
            tasks.push_back([b, e, function] () {
                std::for_each(b, e, function);
            });
            b = e;
            safe_advance(e, end, chunk_size);
        }

        paralell(tp, tasks, callback);
    }

    /**
     * Transform one sequance to an other.
     *
     * This function emulates std::transform, but runs in parallel on the asynchronous task stream.
     *
     * @param tp the task pool to execute on
     * @param istart beginning of the input sequence
     * @param iend the end of the input sequence
     * @param ostart beginning of the output sequence
     * @param transmuter the function to alters the values
     * @param callback the callback called when all tasks are executed
     * @param chunk_size the size of the batches used to form tasks
     *
     * @warning The iterators used must be valid until callback is called.
     *
     * @see parallel
     **/
    template <class InIterator, class OutIterator, class UnaryOperation>
    void transform(task_pool& tp, InIterator istart, InIterator iend, OutIterator ostart, UnaryOperation operation, std::function<void(std::exception_ptr err)> callback, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks = std::list<std::function<void ()>>{};

        auto ib = istart;
        auto ie = ib;
        safe_advance(ie, iend, chunk_size);
        auto ob = ostart;
        while (ie != iend)
        {
            tasks.push_back([ib, ie, ob, operation] () {
                std::transform(ib, ie, ob, operation);
            });
            ib = ie;
            auto n = safe_advance(ie, iend, chunk_size);
            std::advance(ob, n);
        }

        paralell(tp, tasks, callback);
    }

    /**
     * Call a function for each element in a collection..
     *
     * This function emulates std::function, but runs in parallel on the asynchronous task stream.
     *
     * @param tp the task pool to execute on
     * @param start beginning of the sequence
     * @param end the end of the sequence
     * @param generator the function to generate values
     * @param callback the callback called when all tasks are executed
     * @param chunk_size the size of the batches used to form tasks
     *
     * @warning The iterators used must be valid until callback is called.
     *
     * @see parallel
     **/
    template <class InIterator, class OutIterator>
    void copy(task_pool& tp, InIterator istart, InIterator iend, OutIterator ostart, std::function<void(std::exception_ptr err)> callback, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks = std::list<std::function<void ()>>{};

        auto ib = istart;
        auto ie = ib;
        safe_advance(ie, iend, chunk_size);
        auto ob = ostart;
        while (ie != iend)
        {
            tasks.push_back([ib, ie, ob] () {
                std::copy(ib, ie, ob);
            });
            ib = ie;
            auto n = safe_advance(ie, iend, chunk_size);
            std::advance(ob, n);
        }

        paralell(tp, tasks, callback);
    }


    template <class Key, class OutValue>
    struct _map_reduce_state
    {
        std::list<std::pair<Key, OutValue>> mapped;
        std::map<Key, std::list<OutValue>> shuffled;
        std::list<std::pair<Key, OutValue>> reduced;
    };

    template <class InCollection, class OutCollection>
    void map_reduce(task_pool& tp, const InCollection& input, OutCollection& output,
                    std::function<typename OutCollection::value_type(const typename InCollection::value_type)> map,
                    std::function<typename OutCollection::value_type(std::pair<const typename OutCollection::key_type, std::list<typename OutCollection::mapped_type>>)> reduce,
                    std::function<void(std::exception_ptr err)> callback,
                    unsigned int chunk_size = default_chunk_size)
    {
        auto state = std::make_shared<_map_reduce_state<typename OutCollection::key_type, typename OutCollection::mapped_type>>();

        // map
        state->mapped.resize(size(input));
        transform(tp, begin(input), end(input), begin(state->mapped), map, [=, &tp, &output] (std::exception_ptr err) {
            // shuffle
            state->shuffled;
            for (auto pair : state->mapped)
            {
                state->shuffled[pair.first].push_back(pair.second);
            }

            // reduce
            state->reduced.resize(state->shuffled.size());
            transform(tp, begin(state->shuffled), end(state->shuffled), begin(state->reduced), reduce, [=, &output] (std::exception_ptr err) {

                // remap
                output = OutCollection(begin(state->reduced), end(state->reduced));
                callback(std::exception_ptr{});
            }, chunk_size);
        }, chunk_size);

    }
}

#endif
