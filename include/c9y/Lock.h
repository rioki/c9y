
#ifndef _C9Y_LOCK_H_
#define _C9Y_LOCK_H_

namespace c9y
{
    /**
     * Scoped Lock
     *
     * The Lock class implements the scoped lock paradigm. It provides a means
     * to safly lock and unlock objects in an exception safe way. To use the
     * lock class simple put and instance onto the stack, this will lock the
     * object. Once the lock dropes out of scope the object will be unlocked.
     *
     * Exmaple
     * @code
     * Mutex mutex;
     *
     * void f()
     * {
     *     Lock<Mutex> lock(mutex); // mutex.lock();
     *     // do something that is protected by mutex
     * } // mutex.unlock(); (even if exceptions ocur)
     * @endcode
     *
     * @see Mutex
     **/
    template <typename Type>
    class Lock
    {
    public:
        /**
         * Constructor
         *
         * The constructor will lock the given object.
         *
         * @param o the object to lock
         **/
        Lock(Type& o)
        : object(o)
        {
            object.lock();
        }

        /**
         * Destructor.
         *
         * The destructor will unlock the given object.
         **/
        ~Lock()
        {
            object.unlock();
        }

    private:
        Type& object;

        Lock(const Lock<Type>&);
        const Lock<Type>& operator = (const Lock<Type>&);
    };
}

#endif
