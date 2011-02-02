
#ifndef _C9Y_CONDITION_H_
#define _C9Y_CONDITION_H_

#ifdef _WIN32
#include <windows.h>
#endif

namespace c9y
{
    class Condition
    {
    public:
        Condition();

        ~Condition();

        void wait();

        void signal();

    private:
        #ifdef _WIN32
        HANDLE handle;
        unsigned int thread_count;
        #endif

        Condition(const Condition&);
        const Condition& operator = (const Condition&);
    };
}

#endif
