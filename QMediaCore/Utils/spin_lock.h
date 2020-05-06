//
//  spin_lock.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef UTILS_SPIN_LOCK_H
#define UTILS_SPIN_LOCK_H

#include <atomic>

class spin_mutex
{
public:
    spin_mutex() = default;
    ~spin_mutex() = default;
//    spin_lock() :flag_(false/*ATOMIC_FLAG_INIT*/) {}
    inline void lock(){
        while (std::atomic_flag_test_and_set(&flag_)) ;
    }
    inline void unlock(){
        std::atomic_flag_clear(&flag_);
    }
private:
    spin_mutex(const spin_mutex&) = delete;
    spin_mutex& operator=(const spin_mutex&) = delete;
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

template <class mutexT>
class t_lock_guard {
public:
    explicit t_lock_guard(mutexT& mutex):__mutex(mutex) { __mutex.lock(); }
    ~t_lock_guard() { __mutex.unlock(); }
private:
    t_lock_guard(t_lock_guard const&) = delete;
    t_lock_guard& operator=(t_lock_guard const&) = delete;

    mutexT& __mutex;
};

#endif /* UTILS_SPIN_LOCK_H */
