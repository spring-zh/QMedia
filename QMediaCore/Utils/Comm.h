//
//  Comm.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include <float.h>
#include <math.h>
#include <algorithm>
#include <string>
#include <sstream>

#define SAFE_DELETE(p) \
if(p){\
	delete p;\
	p = NULL;\
}

#define SAFE_DELETE_ARRAY(p) \
if(p){\
	delete[] p;\
	p = NULL;\
}

#ifndef MAX
#define MAX(a,b) (a)>(b)?(a):(b)
#endif

#ifndef MIN
#define MIN(a,b) (a)<(b)?(a):(b)
#endif

template <typename T/*,class = typename std::enable_if<std::is_arithmetic<T>::value>::type*/>
class Range
{
public:
	Range() : _start(0), _end(0) {}
    Range(T s, T e) : _start(s), _end(e) {}
	~Range(){}
    
    Range<T> operator+(const T& x){
        return Range(_start + x, _end + x);
    }
    
    Range<T> operator*(const T& x){
        return Range(_start * x, _end * x);
    }
    
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type getLength() const {
        return _end - _start;
    }

    bool isValid() const {return getLength() > 0; }
    
    bool isContain(T val) { return ((val >= _start) && (val <= _end)) ;}

	T _start, _end;
};

// An inheritable class to disallow the copy constructor and operator= functions
class NonCopyable {
protected:
	constexpr NonCopyable() = default;
	~NonCopyable() = default;

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

#define RB_8(ptr) (*((uint8_t*)ptr))
#define RB_16(ptr) ((RB_8(ptr) << 8)|RB_8(ptr+1))
#define RB_24(ptr) ((RB_16(ptr)<<8)|RB_8(ptr+2))
#define RB_32(ptr) ((RB_16(ptr)<<16)|RB_16(ptr+2))
#define RB_64(ptr) (((uint64_t)RB_32(ptr)<<32)|RB_32(ptr+4))

class CallOnce {
public:
    template <class Callable, typename... Args >
    explicit CallOnce(Callable&& __func, Args&&... __args){
        std::forward<Callable>(__func)(std::forward<Args>(__args)...);
//        std::function<void(void)> call = std::bind(__func, __args...);
//        call();
    }
    ~CallOnce() = default;
};

constexpr unsigned BitValue(unsigned n) {
    return 1 << n;
}
inline bool HasValue(unsigned a, unsigned val) { return a & val; }

template <class T>
T StringToValue(const std::string &s){
    T value;
    std::stringstream ss(s);
    ss >> value;
    return value;
}

//FIXME: isn't compatible with float type well
template <class T>
std::string ValueToString(const T &value){
    std::stringstream ss;
    ss << value;
    return ss.str();
}

//template <typename T>
//inline int GetArrayCount(const T array[]){
//    return sizeof(array) / sizeof(T);
//}

#define GET_ARRAY_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

#define FLOAT_ISEQUAL(val, eqVal) (fabs(val - eqVal) < FLT_EPSILON)

#define CLASSREF(c) using c##Ref = std::shared_ptr<c> ;

/** @def DEGREES_TO_RADIANS
 converts degrees to radians
 */
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f) // PI / 180

/** @def RADIANS_TO_DEGREES
 converts radians to degrees
 */
#define RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) // PI * 180
