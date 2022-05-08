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

#include "algorithm.h"

#include <cassert>

namespace c9y
{
    void _sequence_one(task_pool& tp, std::shared_ptr<std::list<std::function<void ()>>> shared_tasks, std::function<void(std::exception_ptr err)> callback)
    {
        assert(shared_tasks);
        assert(!shared_tasks->empty());

        auto task = shared_tasks->front();
        assert(task);
        try
        {
            task();
        }
        catch (...)
        {
            auto err = std::current_exception();
            tp.sync([callback, err] () {
                callback(err);
            });
            return;
        }

        shared_tasks->pop_front();
        if (shared_tasks->empty())
        {
            tp.sync([callback] () {
                callback(std::exception_ptr{});
            });
        }
        else
        {
            tp.async([&tp, shared_tasks, callback] () {
                _sequence_one(tp, shared_tasks, callback);
            });
        }
    }
}