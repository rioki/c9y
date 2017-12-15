// 
// c9y - concurrency
// Copyright(c) 2017 Sean Farrell
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

#ifndef _C9Y_CTRL_FLOW_H_
#define _C9Y_CTRL_FLOW_H_

#include <memory>
#include <functional>
#include <ctime>

#include "defines.h"
#include "task_pool.h"

namespace c9y
{
    class task_pool;

    struct base_handle {
        virtual ~base_handle() {}
    };
    
    /**
     * Handle to a execution context.
     *
     * Some functions, such as start_idle return a handle that can be 
     * used to call stop_idle on. From the users perspective, these handles
     * should be considered opaque.
     **/
    typedef std::shared_ptr<base_handle> handle;
    
    /**
     * Executa a task asyncronously and return the result in a syncronous call.
     *
     * @param task the task to execute
     * @param callback the callback that is called when the task finishes
     *
     * The callback gets two values, an exception_ptr and the resutl value. 
     * If an exception is thrown during the execution of the task the exception
     * is captured and handed of to the callback. 
     * 
     * @see throw_on_error
     **/
    template <typename result_t>
    void async(task_pool& tp, std::function<result_t()> task, std::function<void(std::exception_ptr err, result_t)> callback)
    {
        tp.async([&tp, task, callback]() {
            std::exception_ptr err;
            result_t           res;
            try 
            {
                res = task();
            } 
            catch (...)
            {
                err = std::current_exception();
            }
            tp.sync(std::bind(callback, err, res));
        });
    }

    C9Y_EXPORT
    void async(task_pool& tp, std::function<void()> task, std::function<void(std::exception_ptr err)> callback);

    C9Y_EXPORT
    handle start_idle(task_pool& tp, std::function<bool ()> task);

    C9Y_EXPORT 
    void stop_idle(handle h);

    /**
     * Get current time in ms.
     * 
     * @return time in ms
     * 
     * This function returns a steady wall clock time in ms. The point in time
     * it starts is arbitrary. It is only guaranteed to run steady steady within
     * this program.
     * This is the basis timers use in c9y.
     */
    C9Y_EXPORT
    unsigned int get_ms_time();

    C9Y_EXPORT
    void timer(task_pool& tp, std::function<bool()> task, unsigned int intervall);
    
    /**
     * Convinience fucntion for error handling.
     *
     * Many function return an exception_ptr. If the task threw an exception, is may
     * be sensible to rethrow that exception in the main thread, thus terminating 
     * the execution. 
     **/
    C9Y_EXPORT
    void throw_on_error(std::exception_ptr err);
}

#endif
