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

    template <typename Fn, typename ...Args, typename RetT = typename std::result_of<Fn(Args...)>::type>
    inline Result Wait(Fn fn, Args... args) {
        return WaitInternal(std::bind(fn, args...));
    }

    template <typename Fn, typename ...Args, typename RetT = typename std::result_of<Fn(Args...)>::type>
    inline RetT Run(Fn fn, Args... args) {
        std::function<RetT()> f = std::bind(fn, args...);
        std::unique_lock<std::mutex> lck(mutex_);
        return f();
    }

    template <typename Fn, typename ...Args>
    inline bool Notify(Fn fn, Args... args) {
        std::function<bool()> f = std::bind(fn, args...);
        std::unique_lock<std::mutex> lck(mutex_);
        bool result = f();
        if (result) cv_.notify_one();
        return result;
    }

    template <typename Fn, typename ...Args>
    inline bool NotifyAll(Fn fn, Args... args) {
        std::function<bool()> f = std::bind(fn, args...);
        std::unique_lock<std::mutex> lck(mutex_);
        bool result = f();
        if (result) cv_.notify_all();
        return result;
    }

    inline void Notify() {
        std::unique_lock<std::mutex> lck(mutex_);
        cv_.notify_one();
    }
    
    inline void NotifyNoLock() {
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
