
#ifndef _C9Y_THREADS_H_
#define _C9Y_THREADS_H_

#include <sigc++/slot.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace c9y
{
    class Thread
    {
    public:

        Thread(sigc::slot<void> slot);

        ~Thread();

        void start();

        void join();

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
