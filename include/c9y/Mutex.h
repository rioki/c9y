
#ifndef _C9Y_MUTEX_H_
#define _C9Y_MUTEX_H_

#ifdef _WIN32
#include <windows.h>
#endif

namespace c9y
{
    /**
     * Mututal Exclusion Lock
     *
     * The Mutex class implements a simple mutual exclusion lock. It is the
     * simplest syncronisation construct available. It synconises access to a
     * resource, that can be accessed only from one thread. The locking can be
     * done recusevly from the same thread. Locking and unlocking must be done
     * from the same thread.
     *
     * @see Lock
     **/
    class Mutex
    {
    public:
        /**
         * Constructor
         **/
        Mutex();

        /**
         * Destructor
         **/
        ~Mutex();

        /**
         * Lock the mutex.
         **/
        void lock();

        /**
         * Unlock the mutex,
         **/
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
