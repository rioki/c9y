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
        atasks.push(atask);        
    }

    void task_pool::sync(std::function<void()> stask)
    {
        ref_count++;
        stasks.push(stask);        
    }

    void task_pool::run()
    {
        std::function<void()> stask;

        pool = thread_pool([this] () {execute();}, concurency);
        
        while (ref_count > 0)
        {            
            if (stasks.pop_wait(stask))
            {
                stask();
                ref_count--;
            }
        }

        atasks.wake();

        pool.join();
    }

    void task_pool::execute()
    {
        std::function<void()> atask;

        while (ref_count > 0)
        {
            if (atasks.pop_wait(atask))
            {
                atask();
                ref_count--;
            }
        }
        
        atasks.wake();
        stasks.wake();
    }
}