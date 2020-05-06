//
//  ThreadTask.h
//  QMediaCore
//
//  Created by spring on 2018/8/6.
//  Copyright © 2018年 QMedia. All rights reserved.
//

#ifndef UTILS_THREADTASK_H
#define UTILS_THREADTASK_H

#include "BlockingQueue.h"
#include <thread>

template <typename RetT = void>
class ThreadTask {
public:
    
    typedef typename std::function<RetT()> functmp;
    
    ThreadTask()
    {
        try {
            _thread = std::thread(&ThreadTask::run, this);
        }catch (std::system_error & e){
            return;
        }
    }
    ~ThreadTask()
    {
        functmp nullfunction = nullptr;
        _funcQueue.Put(nullfunction,-1);
        try {
            if (_thread.joinable())
                _thread.join();
        }catch (std::exception & e){
            
        }
    }
    
    template <class func_ptr,class... Args/*,typename = typename std::enable_if<!std::is_void<RetT>::value>::type*/>
    void setTaskCallback(func_ptr func, Args&&... args){
        _callback._function = std::bind(func, args..., std::placeholders::_1,std::placeholders::_2);
    }
    
    template <class func_ptr,class... Args>
    inline void postTask(func_ptr func, Args&&... args){
        _postTask(std::bind(func, args...),-1);
    }
    
    template <class func_ptr,class... Args>
    inline void postTask(int taskId, func_ptr func, Args&&... args){
        _postTask(std::bind(func, args...),taskId);
    }
    
    void removeTaskById(int vId){
        _funcQueue.RemoveById(vId);
    }
    
    void clearTask(){
        _funcQueue.RemoveAll();
    }
    
    inline void _postTask(const functmp& func, int vId){
        _funcQueue.Put(func, vId);
    }
protected:
    ThreadTask(const ThreadTask& that);
    ThreadTask& operator=(const ThreadTask& that);
    
    void run(){
        
        do{
            int vid;
            auto func = _funcQueue.Take(vid);
            if(func){
                _callback.callWithFunction(func,vid);
            }else
                break;
        }while(true);
    }
    
private:
    
    template <typename CbRetT, typename CbTypeT>
    struct Callback {
        std::function<void(CbTypeT,int)> _function;
        void callWithFunction(const functmp& func, int vId){
            RetT ret = func();
            if(_function)_function(ret,vId);
        }
    };
    template <typename CbTypeT>
    struct Callback<void, CbTypeT> {
        void callWithFunction(const functmp& func, int vId){
            func();
        }
    };
    
    std::thread _thread;
    BlockingQueue<functmp> _funcQueue;
    Callback<RetT, RetT> _callback;
};

#endif /* ThreadTask_h */
