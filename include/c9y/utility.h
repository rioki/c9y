
#ifndef _C9Y_UTILITY_H_
#define _C9Y_UTILITY_H_

namespace c9y
{
    /**
     * Suspend Execution for a Defined Duration
     *
     * This function will supend the execution of the current thread for a
     * specified duration.
     *
     * @param ms the ducration in milliseconds
     *
     * @note Depending on your operating system and environment the granularity
     * can be up 10 ms.
     **/
    void sleep(unsigned int ms);
}

#endif
