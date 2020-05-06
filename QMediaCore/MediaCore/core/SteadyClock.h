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
#include "Utils/spin_lock.h"

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
    
	SteadyClock():
    _bPaused(true),
    _startClk(steady_clock::now()),
    _currVal(0) {}
    
    ~SteadyClock() {}

	void setClock(timeT val)
	{
        t_lock_guard<spin_mutex> lck(_spin);
		_currVal = val;
		_startClk = steady_clock::now();
	}
	timeT getClock() const
	{
        t_lock_guard<spin_mutex> lck(_spin);
		if (_bPaused)
		{
			return _currVal;
		}
		else {
			return _currVal + std::chrono::duration_cast<std::chrono::duration<timeT, timeScale>>(steady_clock::now() - _startClk).count();
		}
	}

	void setPaused(bool pause)
	{
        t_lock_guard<spin_mutex> lck(_spin);
        if(pause != _bPaused){
            if(!_bPaused) {
                _currVal += std::chrono::duration_cast<std::chrono::duration<timeT, timeScale>>(steady_clock::now() - _startClk).count();
            }
            _startClk = steady_clock::now();
            _bPaused = pause;
        }
	}
	bool isPaused() const { return _bPaused; }

    
    //get the value form last setClock
//    inline timeT getSetValue() const { return _currVal; }
private:
    mutable spin_mutex _spin;
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

#endif //UTILS_MEDIA_CLOCK_H
