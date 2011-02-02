
#ifndef _C9Y_LOCK_H_
#define _C9Y_LOCK_H_

namespace c9y
{
    template <typename Type>
    class Lock
    {
    public:
        Lock(Type& o)
        : object(o)
        {
            object.lock();
        }

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
