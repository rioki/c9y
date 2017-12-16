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

#include "task_pool.h"

namespace c9y
{
    task_pool::task_pool(size_t c)
    : concurency(c), ref_count(0) {}

    task_pool::~task_pool() {}

    void task_pool::async(std::function<void()> atask)
    {
        ref_count++;
        std::unique_lock<std::mutex> lock(amutex);
        atasks.push(atask);
        acond.notify_one();
    }

    void task_pool::sync(std::function<void()> stask)
    {
        ref_count++;
        std::unique_lock<std::mutex> lock(smutex);
        stasks.push(stask);
        scond.notify_one();
    }

    void task_pool::run()
    {
        pool = thread_pool([this] () {execute();}, concurency);
        
        while (ref_count > 0)
        {
            auto stask = get_next_stask();
            if (stask)
            {
                stask();
                ref_count--;
            }
        }

        acond.notify_all();        

        pool.join();
    }

    void task_pool::execute()
    {
        while (ref_count > 0)
        {
            auto atask = get_next_atask();
            if (atask)
            {
                atask();
                ref_count--;
            }
        }
        
        acond.notify_all();
        scond.notify_all();
    }

    std::function<void()> task_pool::get_next_atask()
    {
        std::unique_lock<std::mutex> lock(amutex);

        if (atasks.empty())
        {
            acond.wait(lock);
        }

        if (!atasks.empty())
        {
            auto atask = atasks.front();
            atasks.pop();
            return atask;
        }
        else
        {
            return std::function<void()>();
        }
    }

    std::function<void()> task_pool::get_next_stask()
    {
        std::unique_lock<std::mutex> lock(smutex);

        if (stasks.empty())
        {
            scond.wait(lock);
        }

        if (!stasks.empty())
        {
            auto stask = stasks.front();
            stasks.pop();
            return stask;
        }
        else
        {
            return std::function<void()>();
        }
    }
}