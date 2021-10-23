// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <shared_mutex>
#include <atomic>

namespace jutils
{
    class jmutex_shared
    {
    public:

        void lock()
        {
            m_WaitingForLock.fetch_add(1, std::memory_order_release);
            m_Mutex.lock();
            m_WaitingForLock.fetch_sub(1, std::memory_order_release);
        }
        void unlock() { m_Mutex.unlock(); }

        void lock_shared()
        {
            while (m_WaitingForLock.load(std::memory_order_acquire) > 0) {}
            m_Mutex.lock_shared();
        }
        void unlock_shared() { m_Mutex.unlock_shared(); }

    private:

        std::shared_mutex m_Mutex;
        std::atomic_uchar m_WaitingForLock = 0;
    };
}
