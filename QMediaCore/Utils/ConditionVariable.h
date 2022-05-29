#include <condition_variable>
#include <mutex>
#include <functional>


class ConditionVariable final {
public:
    enum Result : int {
        OK = 0,
        TIMEOUT = -1
    };

    ConditionVariable() = default;
    ~ConditionVariable() = default;

    void SetTimeoutMs(int32_t timeout_ms = -1) { timeout_ms_ = timeout_ms; }

    template <typename FuncPtr, typename ...Args>
    inline Result Wait(FuncPtr func_ptr, Args... args) {
        return WaitInternal(std::bind(func_ptr, args...));
    }

    template <typename RetT, typename FuncPtr, typename ...Args>
    inline RetT Run(FuncPtr func_ptr, Args... args) {
        std::function<RetT()> f = std::bind(func_ptr, args...);
        std::unique_lock<std::mutex> lck(mutex_);
        return f();
    }

    template <typename FuncPtr, typename ...Args>
    inline bool Notify(FuncPtr func_ptr, Args... args) {
        std::function<bool()> f = std::bind(func_ptr, args...);
        std::unique_lock<std::mutex> lck(mutex_);
        bool result = f();
        if (result) cv_.notify_one();
        return result;
    }

    template <typename FuncPtr, typename ...Args>
    inline bool NotifyAll(FuncPtr func_ptr, Args... args) {
        std::function<bool()> f = std::bind(func_ptr, args...);
        std::unique_lock<std::mutex> lck(mutex_);
        bool result = f();
        if (result) cv_.notify_all();
        return result;
    }

    inline void Notify() {
        std::unique_lock<std::mutex> lck(mutex_);
        cv_.notify_one();
    }

    inline void NotifyAll() {
        std::unique_lock<std::mutex> lck(mutex_);
        cv_.notify_all();
    }

private:

    Result WaitInternal(std::function<bool()> f) {
        std::unique_lock<std::mutex> lck(mutex_);
        int32_t wait_time_ms = 0;
        while (!f()) {
           if(cv_.wait_for(lck, std::chrono::milliseconds(WAIT_PER_TIME_MS)) == std::cv_status::timeout) {
                wait_time_ms += WAIT_PER_TIME_MS;
                if (timeout_ms_ > 0 && wait_time_ms >= timeout_ms_)
                    return TIMEOUT;
            }
        }
        return OK;
    }

    const int32_t WAIT_PER_TIME_MS = 500;
    std::condition_variable cv_;
    std::mutex mutex_;

    int32_t timeout_ms_;
};
