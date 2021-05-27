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

#ifndef _C9Y_parallel_H_
#define _C9Y_parallel_H_

#include "defines.h"

#include <functional>
#include <vector>
#include <list>
#include <algorithm>
#include <map>

#include "latch.h"
#include "task_pool.h"

namespace c9y
{
    //! The default chunking size.
    //!
    //! The STL like algorithms are generally executed with very small
    //! tasks and this may be inefficient to queue each one individually.
    //! As a result they are chunked together. This is the default chunk size
    //! but each has it's own configurable value.
    constexpr auto default_chunk_size = 32u;

    C9Y_EXPORT task_pool& _get_parallel_pool() noexcept;

    //! Execute a tasks in parallel.
    //!
    //! @param begin the beginning of the sequence
    //! @param end the end of the sequence
    template <typename IteratorT>
    void parallel(IteratorT begin, IteratorT end) noexcept
    {
        auto& pool = _get_parallel_pool();
        latch l(std::distance(begin, end));
        for (auto i = begin; i != end; ++i)
        {
            auto task = *i;
            pool.enqueue([&, task] () {
                task();
                l.count_down();
            });
        }
        l.wait();
    }

    //! Execute a tasks in parallel.
    //!
    //! @param begin the beginning of the sequence
    //! @param end the end of the sequence
    //! @param tasks a collection of task
    inline void parallel(const std::list<std::function<void ()>>& tasks) noexcept
    {
        parallel(begin(tasks), end(tasks));
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

    //! Generate a number of values.
    //!
    //! This function emulates std::generate, but runs in parallel.
    //!
    //! @param start beginning of the sequence
    //! @param end the end of the sequence
    //! @param generator the function to generate values
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    template <class Iterator, class Generator>
    void parallel_generate(Iterator start, Iterator end, Generator generator, unsigned int chunk_size = default_chunk_size)
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

        parallel(tasks);
    }


    //! Transform one sequance to an other.
    //!
    //! This function emulates std::transform, but runs in parallel.
    //!
    //! @param istart beginning of the input sequence
    //! @param iend the end of the input sequence
    //! @param ostart beginning of the output sequence
    //! @param transmuter the function to alters the values
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    template <class InIterator, class OutIterator, class UnaryOperation>
    void parallel_transform(InIterator istart, InIterator iend, OutIterator ostart, UnaryOperation operation, unsigned int chunk_size = default_chunk_size)
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

        parallel(tasks);
    }

    //! Copy one sequance to an other.
    //!
    //! This function emulates std::copy, but runs in parallel.
    //!
    //! @param istart beginning of the input sequence
    //! @param iend the end of the input sequence
    //! @param ostart beginning of the output sequence
    //! @param transmuter the function to alters the values
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    template <class InIterator, class OutIterator>
    void parallel_copy(InIterator istart, InIterator iend, OutIterator ostart, unsigned int chunk_size = default_chunk_size)
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

        parallel(tasks);
    }

    template <class Key, class OutValue>
    struct _map_reduce_state
    {
        std::list<std::pair<Key, OutValue>> mapped;
        std::map<Key, std::list<OutValue>> shuffled;
        std::list<std::pair<Key, OutValue>> reduced;
    };

    template <class InCollection, class OutCollection>
    void parallel_map_reduce(const InCollection& input, OutCollection& output,
                             std::function<typename OutCollection::value_type(const typename InCollection::value_type)> map,
                             std::function<typename OutCollection::value_type(std::pair<const typename OutCollection::key_type, std::list<typename OutCollection::mapped_type>>)> reduce,
                             unsigned int chunk_size = default_chunk_size)
    {
        auto state = std::make_shared<_map_reduce_state<typename OutCollection::key_type, typename OutCollection::mapped_type>>();

        // map
        state->mapped.resize(size(input));
        parallel_transform(begin(input), end(input), begin(state->mapped), map, chunk_size);

        // shuffle
        state->shuffled;
        for (auto pair : state->mapped)
        {
            state->shuffled[pair.first].push_back(pair.second);
        }

        // reduce
        state->reduced.resize(state->shuffled.size());
        parallel_transform(begin(state->shuffled), end(state->shuffled), begin(state->reduced), reduce, chunk_size);

        // remap
        output = OutCollection(begin(state->reduced), end(state->reduced));
    }
}

#endif
