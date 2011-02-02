
#ifndef _C9Y_MUTEX_H_
#define _C9Y_MUTEX_H_

#ifdef _WIN32
#include <windows.h>
#endif

namespace c9y
{
    class Mutex
    {
    public:
        Mutex();

        ~Mutex();

        void lock();

        void unlock();

    private:
        #ifdef _WIN32
        HANDLE handle;
        #endif

        Mutex(const Mutex&);
        const Mutex& operator = (const Mutex&);
    };
}

#endif
