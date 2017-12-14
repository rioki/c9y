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

#ifndef _C9Y_TASK_POOL_H_
#define _C9Y_TASK_POOL_H_

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>

#include "defines.h"

#include "thread_pool.h"

namespace c9y
{
    class C9Y_EXPORT task_pool
    {
    public:
        
        task_pool(size_t concurency = std::thread::hardware_concurrency());

        task_pool(const task_pool&) = delete;

        ~task_pool();

        task_pool& operator = (const task_pool&) = delete;

        void async(std::function<void ()> task);

        void sync(std::function<void()> task);

        void run();
        
    private:
        size_t                            concurency;
        std::atomic<unsigned int>         ref_count;

        std::mutex                        amutex;
        std::condition_variable           acond;
        std::queue<std::function<void()>> atasks;

        std::mutex                        smutex;
        std::condition_variable           scond;
        std::queue<std::function<void()>> stasks;

        thread_pool                       pool;

        void execute();

        std::function<void()> get_next_atask();
        std::function<void()> get_next_stask();
    };
}

#endif
