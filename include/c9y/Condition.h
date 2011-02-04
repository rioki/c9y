
#ifndef _C9Y_CONDITION_H_
#define _C9Y_CONDITION_H_

#ifdef _WIN32
#include <windows.h>
#endif

namespace c9y
{
    /**
     * Condition
     *
     * The Condition class implements a means of inter thread comunication to
     * signal if a given condition is met or not.
     *
     * @note The concept has more in common with Windows' events than POSIX's
     * condition variables.
     **/
    class Condition
    {
    public:
        /**
         * Constructor
         *
         * Create a condition witch is in the clear state.
         **/
        Condition();

        /**
         * Destrcutor
         **/
        ~Condition();

        /**
         * Wait on the condition to be met.
         *
         * This method will wait until the condition is met. If the condition
         * already is met, there obvioulsy will be no wait.
         **/
        void wait();

        /**
         * Signal the Condition
         *
         * This method will notify all waiting threads that the condition is
         * met and wake them up apropriately.
         **/
        void signal();

        /**
         * Clear the Condition
         *
         * This method will set the condition so it is not met. Any thread that
         * will wait on the condition will block.
         **/
        void clear();

    private:
        #ifdef _WIN32
        HANDLE handle;
        #endif

        Condition(const Condition&);
        const Condition& operator = (const Condition&);
    };
}

#endif
