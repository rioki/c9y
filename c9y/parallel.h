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

#ifndef _C9Y_PARALELL_H_
#define _C9Y_PARALELL_H_

#include "defines.h"

#include <functional>
#include <vector>
#include <list>
#include <numeric>
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
    constexpr size_t default_chunk_size = 32u;

    C9Y_EXPORT task_pool& _get_parallel_pool() noexcept;

    inline size_t _get_results_size(const size_t size, const size_t chunk_size)
    {
        if (size % chunk_size == 0)
        {
            return size / chunk_size;
        }
        else
        {
            return size / chunk_size + 1;
        }
    }

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
    inline void parallel(const std::vector<std::function<void ()>>& tasks) noexcept
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

    //! Checks if unary predicate returns true for all elements in the range.
    //!
    //! This function emulates std::all_of, but runs in parallel.
    //!
    //! @param first beginning of the sequence
    //! @param last the end of the sequence
    //! @param predicate the function is called for each element
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    template <class InIterator, class UnaryOperation>
    bool parallel_all_of(InIterator first, InIterator last, UnaryOperation predicate, size_t chunk_size = default_chunk_size)
    {
        auto tasks   = std::vector<std::function<void ()>>{};
        auto results = std::vector<bool>(_get_results_size(std::distance(first, last), chunk_size), false);

        auto b  = first;
        auto e  = b;
        auto ri = 0;
        safe_advance(e, last, chunk_size);
        while (b != last)
        {
            tasks.push_back([b, e, ri, &results, &predicate] () {
                results[ri] = std::all_of(b, e, predicate);
            });
            b = e;
            safe_advance(e, last, chunk_size);
            ri++;
        }

        parallel(tasks);

        return std::all_of(begin(results), end(results), [] (const auto& v) {return v;});
    }

    //! Checks if unary predicate returns true for at least one element in the range.
    //!
    //! This function emulates std::none_of, but runs in parallel.
    //!
    //! @param first beginning of the sequence
    //! @param last the end of the sequence
    //! @param predicate the function is called for each element
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    template <class InIterator, class UnaryOperation>
    bool parallel_any_of(InIterator first, InIterator last, UnaryOperation predicate, size_t chunk_size = default_chunk_size)
    {
        auto tasks   = std::vector<std::function<void ()>>{};
        auto results = std::vector<bool>(_get_results_size(std::distance(first, last), chunk_size), false);

        auto b  = first;
        auto e  = b;
        auto ri = 0;
        safe_advance(e, last, chunk_size);
        while (b != last)
        {
            tasks.push_back([b, e, ri, &results, &predicate] () {
                results[ri] = std::any_of(b, e, predicate);
            });
            b = e;
            safe_advance(e, last, chunk_size);
            ri++;
        }

        parallel(tasks);

        return std::any_of(begin(results), end(results), [] (const auto& v) {return v;});
    }

    //! Checks if unary predicate returns true for no elements in the range .
    //!
    //! This function emulates std::none_of, but runs in parallel.
    //!
    //! @param first beginning of the sequence
    //! @param last the end of the sequence
    //! @param predicate the function is called for each element
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    template <class InIterator, class UnaryOperation>
    bool parallel_none_of(InIterator first, InIterator last, UnaryOperation predicate, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks   = std::vector<std::function<void ()>>{};
        auto results = std::vector<bool>(_get_results_size(std::distance(first, last), chunk_size));

        auto b  = first;
        auto e  = b;
        auto ri = 0;
        safe_advance(e, last, chunk_size);
        while (b != last)
        {
            tasks.push_back([b, e, ri, &results, &predicate] () {
                results[ri] = std::none_of(b, e, predicate);
            });
            b = e;
            safe_advance(e, last, chunk_size);
            ri++;
        }

        parallel(tasks);

        // Once we found the sequences that match none_of all of them must be true,
        // for all of them to be true.
        return std::all_of(begin(results), end(results), [] (const auto& v) {return v;});
    }

    //! Counts the elements that are equal to value.
    //!
    //! This function emulates std::count, but runs in parallel.
    //!
    //! @param first beginning of the sequence
    //! @param last the end of the sequence
    //! @param value the value to count
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    template <class Iterator, class Type>
    size_t parallel_count(Iterator first, Iterator last, Type value, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks   = std::vector<std::function<void ()>>{};
        auto results = std::vector<size_t>(_get_results_size(std::distance(first, last), chunk_size), 0u);

        auto b  = first;
        auto e  = b;
        auto ri = 0;
        safe_advance(e, last, chunk_size);
        while (b != last)
        {
            tasks.push_back([b, e, ri, &results, &value] () {
                results[ri] = std::count(b, e, value);
            });
            b = e;
            safe_advance(e, last, chunk_size);
            ri++;
        }

        parallel(tasks);

        return std::accumulate(begin(results), end(results), size_t{0u});
    }

    //! counts elements for which predicate returns true.
    //!
    //! This function emulates std::count, but runs in parallel.
    //!
    //! @param first beginning of the sequence
    //! @param last the end of the sequence
    //! @param predicate the function is called for each element
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    template <class Iterator, class UnaryOperation>
    size_t parallel_count_if(Iterator first, Iterator last, UnaryOperation predicate, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks   = std::vector<std::function<void ()>>{};
        auto results = std::vector<size_t>(_get_results_size(std::distance(first, last), chunk_size), 0u);

        auto b  = first;
        auto e  = b;
        auto ri = 0;
        safe_advance(e, last, chunk_size);
        while (b != last)
        {
            tasks.push_back([b, e, ri, &results, &predicate] () {
                results[ri] = std::count_if(b, e, predicate);
            });
            b = e;
            safe_advance(e, last, chunk_size);
            ri++;
        }

        parallel(tasks);

        return std::accumulate(begin(results), end(results), size_t{0u});
    }

    //! Reduces the range, possibly permuted and aggregated in unspecified manner.
    //!
    //! This function emulates std::count, but runs in parallel.
    //!
    //! @param first beginning of the sequence
    //! @param last the end of the sequence
    //! @param init the initial value
    //! @param binary_op operator used to combine two values
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    //! @{
    template <class Iterator, class Type>
    Type parallel_reduce(Iterator first, Iterator last, Type init, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks   = std::vector<std::function<void ()>>{};
        auto results = std::vector<Type>(_get_results_size(std::distance(first, last), chunk_size), init);

        auto b  = first;
        auto e  = b;
        auto ri = 0;
        safe_advance(e, last, chunk_size);
        while (b != last)
        {
            tasks.push_back([b, e, ri, &results, &init] () {
                results[ri] = std::reduce(b, e, init);
            });
            b = e;
            safe_advance(e, last, chunk_size);
            ri++;
        }

        parallel(tasks);

        return std::reduce(begin(results), end(results), init);
    }

    template <class Iterator, class Type, class BinaryOperator>
    Type parallel_reduce(Iterator first, Iterator last, Type init, BinaryOperator binary_op, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks   = std::vector<std::function<void ()>>{};
        auto results = std::vector<Type>(_get_results_size(std::distance(first, last), chunk_size), init);

        auto b  = first;
        auto e  = b;
        auto ri = 0;
        safe_advance(e, last, chunk_size);
        while (b != last)
        {
            tasks.push_back([b, e, ri, &results, &init, &binary_op] () {
                results[ri] = std::reduce(b, e, init, binary_op);
            });
            b = e;
            safe_advance(e, last, chunk_size);
            ri++;
        }

        parallel(tasks);

        return std::reduce(begin(results), end(results), init, binary_op);
    }
    //! @}

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
        auto tasks = std::vector<std::function<void ()>>{};

        auto b = start;
        auto e = b;
        safe_advance(e, end, chunk_size);
        while (b != end)
        {
            tasks.push_back([b, e, &generator] () {
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
    //! @param operation the function to alters the values
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    template <class InIterator, class OutIterator, class UnaryOperation>
    void parallel_transform(InIterator istart, InIterator iend, OutIterator ostart, UnaryOperation operation, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks = std::vector<std::function<void ()>>{};

        auto ib = istart;
        auto ie = ib;
        safe_advance(ie, iend, chunk_size);
        auto ob = ostart;
        while (ib != iend)
        {
            tasks.push_back([ib, ie, ob, &operation] () {
                std::transform(ib, ie, ob, operation);
            });
            ib = ie;
            auto n = safe_advance(ie, iend, chunk_size);
            std::advance(ob, n);
        }

        parallel(tasks);
    }

    //! Execture a function for each element in a sequence.
    //!
    //! This function emulates std::for_rach, but runs in parallel.
    //!
    //! @param start beginning of the sequence
    //! @param end the end of the sequence
    //! @param operation the function is called for each element
    //! @param chunk_size the size of the batches used to form tasks
    //!
    //! @see parallel
    template <class InIterator, class UnaryOperation>
    void parallel_for_each(InIterator start, InIterator end, UnaryOperation operation, unsigned int chunk_size = default_chunk_size)
    {
        auto tasks = std::vector<std::function<void ()>>{};

        auto b = start;
        auto e = b;
        safe_advance(e, end, chunk_size);
        while (b != end)
        {
            tasks.push_back([b, e, &operation] () {
                std::for_each(b, e, operation);
            });
            b = e;
            auto n = safe_advance(e, end, chunk_size);
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
        auto tasks = std::vector<std::function<void ()>>{};

        auto ib = istart;
        auto ie = ib;
        safe_advance(ie, iend, chunk_size);
        auto ob = ostart;
        while (ib != iend)
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

    //! Map Reduce in Paralell
    //!
    //! This function will map and reduce input using the map / deuce algorithm using as many threads
    //! as sensibly usefull.
    //!
    //! @param input input collection
    //! @param output output collection
    //! @param map the function to map from value to key
    //! @param reduce the function to reduce from key to result
    //! @param chunk_size the size of the batches used to form tasks
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
        for (const auto& [key, value] : state->mapped)
        {
            state->shuffled[key].push_back(value);
        }

        // reduce
        state->reduced.resize(state->shuffled.size());
        parallel_transform(begin(state->shuffled), end(state->shuffled), begin(state->reduced), reduce, chunk_size);

        // remap
        output = OutCollection(begin(state->reduced), end(state->reduced));
    }
}

#endif
