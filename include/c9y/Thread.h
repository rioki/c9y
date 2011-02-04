
#ifndef _C9Y_THREADS_H_
#define _C9Y_THREADS_H_

#include <sigc++/slot.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace c9y
{
    /**
     * Thread of Execution
     *
     * The Thread class implements a wrapper around the thread concept.
     **/
    class Thread
    {
    public:
        /**
         * Constructor
         *
         * @param slot The slot that will be executed in the other thread.
         *
         * @note The constructor will not start thread execution use start() to
         * start thread execution.
         **/
        Thread(sigc::slot<void> slot);

        /**
         * Destructor
         *
         * @warning If the thread is still executing, it will be killed. You
         * whould wait (Thread::wait) for the thread to properly finish
         * execution.
         **/
        ~Thread();

        /**
         * Start the thread.
         **/
        void start();

        /**
         * Wait for the thread to finish execution.
         *
         * @todo rename to wait
         **/
        void join();

        /**
         * Forcefully stop the thread execution.
         *
         * This method will instruct the operating system to stop the execution
         * of the thread. This is a very servere operation and will probably
         * result in loss of data, since the stack is not properly unwound. You
         * should use other means to signal the thread to end and wait for it's
         * proper termination.
         **/
        void kill();

    private:
        sigc::slot<void> slot;
        #ifdef _WIN32
        HANDLE handle;
        DWORD pid;
        friend DWORD WINAPI threadproc(LPVOID lpParameter);
        #endif

        Thread(const Thread&);
        const Thread& operator = (const Thread&);
    };
}

#endif
