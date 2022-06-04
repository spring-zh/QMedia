//
//  ThreadTaskFuture.h
//  QMediaCore
//
//  Created by spring on 2018/8/6.
//  Copyright © 2018年 QMedia. All rights reserved.
//

#ifndef UTILS_THREADTASK_FUTURE_H_
#define UTILS_THREADTASK_FUTURE_H_

#include "BlockingQueue.h"
#include <thread>
#include <future>

#if defined(_PTHREAD_H) && (defined(__GNUC__) || defined(__clang__))
#define SET_CURRENT_THREAD_NAME(n) pthread_setname_np(n);
#else
#define SET_CURRENT_THREAD_NAME(n) n
#endif

#if __EXCEPTIONS
#define CATCH_BEGIN \
try {
#define CATCH_END \
} catch (std::system_error & e){ \
}
#else
#define CATCH_BEGIN
#define CATCH_END
#endif

class ThreadTaskFuture final {
public:

    ThreadTaskFuture(std::string name = ""): name_(name) {
        thread_ = std::thread(&ThreadTaskFuture::run, this);
    }
    
    ~ThreadTaskFuture() {
        WaitForStop();
    }
    
    bool IsCurrent() const {
        return (thread_.get_id() == std::this_thread::get_id());
    }
    
    template <typename Fn, class... Args, typename RetT = typename std::result_of<Fn(Args...)>::type>
    inline std::future<RetT> PostTask(Fn&& fn, Args&&... args) {
        return PostTaskInternal<RetT>(TASK_IMPL<RetT>(new TaskImpl<RetT>(std::bind(fn, std::forward<Args>(args)...))));
    }
    
    template <typename Fn, class... Args, typename RetT = typename std::result_of<Fn(Args...)>::type>
    inline std::future<RetT> PostTask(int taskId, Fn&& fn, Args&&... args) {
        return PostTaskInternal<RetT>(TASK_IMPL<RetT>(new TaskImpl<RetT>(std::bind(fn, std::forward<Args>(args)...))), taskId);
    }

    template <typename Fn, class... Args, typename RetT = typename std::result_of<Fn(Args...)>::type>
    inline std::future<RetT> InsertTask(Fn&& fn, Args&&... args) {
        return InsertTaskInternal<RetT>(TASK_IMPL<RetT>(new TaskImpl<RetT>(std::bind(fn, std::forward<Args>(args)...))));
    }
    
    template <typename Fn, class... Args, typename RetT = typename std::result_of<Fn(Args...)>::type>
    inline std::future<RetT> InsertTask(int taskId, Fn&& fn, Args&&... args) {
        return InsertTaskInternal<RetT>(TASK_IMPL<RetT>(new TaskImpl<RetT>(std::bind(fn, std::forward<Args>(args)...))), taskId);
    }
    
    inline void RemoveTask(int vId = -1) {
        task_queue_.RemoveById(vId);
    }

    inline std::future<void> RemoveTaskInThread(int vId = -1) {
        return InsertTask(&BlockingQueue<TASK>::RemoveById, &task_queue_, vId);
    }
    
    inline void ClearTasks() {
        task_queue_.RemoveAll();
    }
    
    inline std::future<void> ClearTasksInThread() {
        return InsertTask(&BlockingQueue<TASK>::RemoveAll, &task_queue_);
    }

protected:

    void WaitForStop() {
        task_queue_.Put(nullptr,-1);
        if (thread_.joinable())
            thread_.join();
    }
    
    class Task {
    public:
        virtual ~Task() {}
        virtual void Run() = 0;
    };
    
    template <typename CbRetT>
    class TaskImpl : public Task {
    public:
        typedef typename std::function<CbRetT()> FUNCTION;
        explicit TaskImpl(FUNCTION func):func_(func) {}
        inline void Run() override { promise_.set_value(func_()); }
        
        FUNCTION func_;
        std::promise<CbRetT> promise_;
    };
    
    template <>
    class TaskImpl<void> : public Task {
    public:
        typedef typename std::function<void()> FUNCTION;
        explicit TaskImpl(FUNCTION func):func_(func) {}
        inline void Run() override { func_(); promise_.set_value(); }
        
        FUNCTION func_;
        std::promise<void> promise_;
    };

    using TASK = std::shared_ptr<Task>;

    template <typename RetT>
    using TASK_IMPL = std::shared_ptr<TaskImpl<RetT>>;

    template <typename RetT>
    std::future<RetT> PostTaskInternal(const TASK_IMPL<RetT>& task, int vId = -1) {
        task_queue_.Put(task, vId);
        return task->promise_.get_future();
    }

    template <typename RetT>
    std::future<RetT> InsertTaskInternal(const TASK_IMPL<RetT>& task, int vId = -1) {
        task_queue_.Insert(task, vId);
        return task->promise_.get_future();
    }

    ThreadTaskFuture(const ThreadTaskFuture& that);
    ThreadTaskFuture& operator=(const ThreadTaskFuture& that);
    
    void run() {
        SET_CURRENT_THREAD_NAME(name_.c_str());
        do {
            int vid;
            auto task = task_queue_.Take(vid);
            if(task) {
                task->Run();
            } else
                break;
        } while(true);
    }
    
private:
    std::string name_;
    std::thread thread_;
    BlockingQueue<TASK> task_queue_;
};

#endif /* UTILS_THREADTASK_FUTURE_H */
