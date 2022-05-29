//
//  SteadyClock.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef UTILS_MEDIA_CLOCK_H
#define UTILS_MEDIA_CLOCK_H

#include <chrono>
#include <iomanip>
#include <sstream>

namespace LockFree {
class SpinMutex {
public:
    SpinMutex() = default;
    ~SpinMutex() = default;

    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire));
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

template <class mutexT>
class LockGuard {
public:
    explicit LockGuard(mutexT& mutex):__mutex(mutex) { __mutex.lock(); }
    ~LockGuard() { __mutex.unlock(); }
private:
    LockGuard(LockGuard const&) = delete;
    LockGuard& operator=(LockGuard const&) = delete;

    mutexT& __mutex;
};
}

//timeScale template
using scale_nanoseconds = std::nano;
using scale_microseconds = std::micro;
using scale_milliseconds = std::milli;
using scale_seconds = std::ratio<1>;
using scale_minutes = std::ratio<60>;
using scale_hours = std::ratio<3600>;

//choose your time type and timscale unit
template <typename timeT = float, class timeScale = scale_seconds>
class SteadyClock final
{
public:
    using steady_clock = std::chrono::steady_clock;

    #define DURATION_CAST std::chrono::duration_cast<std::chrono::duration<timeT, timeScale>>
    
	SteadyClock(): _bPaused(true), _startClk(steady_clock::now()), _currVal(0) {}
    ~SteadyClock() {}

	void SetClock(timeT val) {
        LockFree::LockGuard<LockFree::SpinMutex> lck(spin_mutex_);
		_currVal = val;
		_startClk = steady_clock::now();
	}
	timeT GetClock() const {
        LockFree::LockGuard<LockFree::SpinMutex> lck(spin_mutex_);
		if (_bPaused) {
			return _currVal;
		}
		else {
			return _currVal + DURATION_CAST(steady_clock::now() - _startClk).count();
		}
	}

	void SetPaused(bool pause) {
        LockFree::LockGuard<LockFree::SpinMutex> lck(spin_mutex_);
        if(pause != _bPaused){
            if(!_bPaused) {
                _currVal += DURATION_CAST(steady_clock::now() - _startClk).count();
            }
            _startClk = steady_clock::now();
            _bPaused = pause;
        }
	}
	bool IsPaused() const { return _bPaused; }

private:
    mutable LockFree::SpinMutex spin_mutex_;
    timeT _currVal;
	bool _bPaused;
	steady_clock::time_point _startClk;
};

class SystemClock {
public:
    using steady_clock = std::chrono::steady_clock;
    using system_clock = std::chrono::system_clock;
    
    template <typename timeT = float, class timeScale = scale_seconds>
    inline static timeT getCurrentTime() {
        return std::chrono::duration_cast<std::chrono::duration<timeT, timeScale>>(steady_clock::now().time_since_epoch()).count();
    }
    
    inline static std::string getTimeFormatString(){
        auto t = system_clock::to_time_t(system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H-%M-%S");
        return ss.str();
    }
};

//class TimeDurationPrint {
//public:
//	TimeDurationPrint(const char* logstr) {
//		logstring = logstr;
//		firstTimeMs = SystemClock::getCurrentTime<int64_t ,scale_milliseconds >();
//	}
//	~TimeDurationPrint() {
//		int64_t secondTimeMs = SystemClock::getCurrentTime<int64_t ,scale_milliseconds >();
//		LOGI("%s Time Duration : %lld milliseconds", logstring.c_str(), (long long)secondTimeMs - firstTimeMs);
//	}
//	std::string logstring;
//	int64_t firstTimeMs;
//};

#endif //UTILS_MEDIA_CLOCK_H
