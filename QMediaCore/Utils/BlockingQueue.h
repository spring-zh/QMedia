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

template <typename T>
class BlockingQueue{
public:
    BlockingQueue() {}
    virtual ~BlockingQueue() {}

    static_assert(std::is_copy_assignable<T>::value &&
                  std::is_move_assignable<T>::value, "template class is not copyable!!");
    static_assert(sizeof(T) < 1024, "template class size is too large!!!");
    
    typedef typename std::lock_guard<std::mutex> guard_lock;

    void Put(const T& value, int vId)
    {
        guard_lock lock(mtx_);
        ItemT item = {value,vId};
        queue_.push_back(item);
        cond_.notify_one();
    }

    T Take(int& vIdOut)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        for (;;)
        {
            if (!queue_.empty())
                break;
            cond_.wait(lock, [this]{return !queue_.empty ();});
        }
        
        ItemT rtItem = queue_.front();
        queue_.pop_front();
        vIdOut = rtItem._id;
        return rtItem._t;
    }

    size_t Size() const
    {
        guard_lock lock (mtx_);
        return queue_.size();
    }
    
    void RemoveAll()
    {
        guard_lock lock(mtx_);
        queue_.clear();
    }
    
    void RemoveById(int vId)
    {
        guard_lock lock(mtx_);
        for(auto iter = queue_.begin();iter != queue_.end(); ){
            if (iter->_id == vId) {
                iter = queue_.erase(iter);
            }else
                ++iter;
        }
    }
private:
    BlockingQueue (const BlockingQueue& rhs);
    BlockingQueue& operator = (const BlockingQueue& rhs);
private:
    struct ItemT {
        T _t;
        int _id;
    };
    std::mutex mtx_;
    std::condition_variable cond_;
    std::list<ItemT> queue_;
};

#endif //UTILS_BLOCKINGQUEUE_H
