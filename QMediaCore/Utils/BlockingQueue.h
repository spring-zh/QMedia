//
//  BlockingQueue.h
//  QMediaCore
//
//  Created by spring on 2018/8/6.
//  Copyright © 2018年 QMedia. All rights reserved.
//

#ifndef UTILS_BLOCKINGQUEUE_H
#define UTILS_BLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <list>

template <typename T, typename = typename std::enable_if<std::is_copy_assignable<T>::value>::type>
class BlockingQueue {
public:
    static_assert(sizeof(T) < 1024, "template class size is too large!!!");

    BlockingQueue() {}
    virtual ~BlockingQueue() {}
    
    typedef typename std::unique_lock<std::mutex> unique_lock;

    void Put(const T& value, int vId) {
        unique_lock lock(mutex_);
        ObjectT item = {value,vId};
        queue_.push_back(item);
        cv_.notify_one();
    }

    void Insert(const T& value, int vId) {
        unique_lock lock(mutex_);
        ObjectT obj = {value,vId};
        queue_.push_front(obj);
        cv_.notify_one();
    }

    T Take(int& vIdOut) {
        unique_lock lock(mutex_);
        for (;;) {
            if (!queue_.empty()) break;
            cv_.wait(lock, [this]{return !queue_.empty();});
        }
        
        ObjectT ret_obj = queue_.front();
        queue_.pop_front();
        vIdOut = ret_obj._id;
        return ret_obj._t;
    }

    size_t Size() const {
        unique_lock lock (mutex_);
        return queue_.size();
    }
    
    void RemoveAll() {
        unique_lock lock(mutex_);
        queue_.clear();
    }
    
    void RemoveById(int vId) {
        unique_lock lock(mutex_);
        queue_.remove_if([vId](const ObjectT& obj){return obj._id == vId;});
    }

private:
    BlockingQueue (const BlockingQueue& rhs);
    BlockingQueue& operator = (const BlockingQueue& rhs);
private:
    struct ObjectT {
        T _t;
        int _id;
    };
    std::mutex mutex_;
    std::condition_variable cv_;
    std::list<ObjectT> queue_;
};

#endif //UTILS_BLOCKINGQUEUE_H
