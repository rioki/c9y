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

#include "Condition.h"

#include <stdexcept>

namespace c9y
{
//------------------------------------------------------------------------------
    Condition::Condition()
    : signaled(false)
    {
        int ret = pthread_mutex_init(&mutex, NULL);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to create mutext.");
        }
        
        ret = pthread_cond_init(&cond, NULL);
        if (ret != 0)
        {
             throw std::runtime_error("Failed to create condition.");
        }
    }

//------------------------------------------------------------------------------
    Condition::~Condition()
    {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

//------------------------------------------------------------------------------
    void Condition::wait()
    {
        int ret = pthread_mutex_lock(&mutex);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to lock error.");
        }
        
        if (! signaled)
        {
            ret = pthread_cond_wait(&cond, &mutex);
            if (ret != 0)
            {
                pthread_mutex_unlock(&mutex);
                throw std::runtime_error("Failed to wait on condition.");
            }
        }
        
        ret = pthread_mutex_unlock(&mutex);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to unlock mutex.");
        }
    }

//------------------------------------------------------------------------------
    void Condition::signal()
    {
        int ret = pthread_mutex_lock(&mutex);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to lock error.");
        }
        
        signaled = true;
        
        ret = pthread_mutex_unlock(&mutex);
        if (ret != 0)
        {
            pthread_mutex_unlock(&mutex);
            throw std::runtime_error("Failed to unlock mutex.");
        }
        
        ret = pthread_cond_broadcast(&cond);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to signal condition.");
        }
    }

//------------------------------------------------------------------------------
    void Condition::clear()
    {
        int ret = pthread_mutex_lock(&mutex);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to lock error.");
        }
        
        signaled = false;
        
        ret = pthread_mutex_unlock(&mutex);
        if (ret != 0)
        {
            throw std::runtime_error("Failed to unlock mutex.");
        }
    }
}
