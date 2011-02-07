//
// c9y - concurrency
// Copyright 2011 Sean Farell
//
// This file is part of c9y.
//
// c9y is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// c9y is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with c9y. If not, see <http://www.gnu.org/licenses/>.
//


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
