//
//  rw_lock.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef UTILS_RW_LOCK_H
#define UTILS_RW_LOCK_H

#if (_WIN32_WINNT >= 0x0600)
typedef SRWLOCK rwlock_t;
#define rwlock_init(m) InitializeSRWLock(m)
#define rwlock_rdlock(m) AcquireSRWLockShared(m)
#define rwlock_rdunlock(m) ReleaseSRWLockShared(m)
#define rwlock_wrlock(m) AcquireSRWLockExclusive(m)
#define rwlock_wrunlock(m) ReleaseSRWLockExclusive(m)
#define rwlock_release(m) (m)
#endif
//#if defined(__POSIX_LIB__)
#include <pthread.h>
typedef pthread_rwlock_t rwlock_t;
#define rwlock_init(m) pthread_rwlock_init(m,NULL)
#define rwlock_rdlock(m) pthread_rwlock_rdlock(m)
#define rwlock_rdunlock(m) pthread_rwlock_unlock(m)
#define rwlock_wrlock(m) pthread_rwlock_wrlock(m)
#define rwlock_wrunlock(m) pthread_rwlock_unlock(m)
#define rwlock_release(m) pthread_rwlock_destroy(m)
//#endif

class rwlock{
public:
    rwlock(){ rwlock_init(&_lock); }
    ~rwlock(){ rwlock_release(&_lock); }

    inline bool rdlock(){ return rwlock_rdlock(&_lock) == 0; }

    inline bool rdunlock(){ return rwlock_rdunlock(&_lock) == 0; }

    inline bool wrlock(){ return rwlock_wrlock(&_lock) == 0; }

    inline bool wrunlock(){ return rwlock_wrunlock(&_lock) == 0; }

private:
    rwlock_t _lock;
};

class rdlock_guard{
public:
    explicit rdlock_guard(rwlock& lck):__lck(lck) { __lck.rdlock(); }
    ~rdlock_guard(){ __lck.rdunlock(); }
private:
    rwlock& __lck;
};

class wrlock_guard{
public:
    explicit wrlock_guard(rwlock& lck):__lck(lck){ __lck.wrlock(); }
    ~wrlock_guard(){ __lck.wrunlock(); }
private:
    rwlock& __lck;
};

#endif /* UTILS_RW_LOCK_H */
