//
//  Logger.h
//  QMediaCore
//
//  Created by LINQIUMING on 17/05/2017.
//  Copyright © 2019 QMedia. All rights reserved.
//

/**
 * add the following lines at the very begining at your cpp file to custom your own log tag. if this 2 line are not added or uncommited no log will printed.
 *
 * #define LOG_TAG YOUR_TAG
 * #define LOG_NDEBUG 0
 *
 * for example:in MediaManager_jni.cpp
 * add this:
 * +#define LOG_NDEBUG 0
 * +#define LOG_TAG "MediaManager_jni"
 * */

#ifndef Logger_hpp
#define Logger_hpp

#include "PlatformConfig.h"
#include <string.h>

extern int global_log_level;

void initLogFile(const char *path);
void destroyLogFile();

/**
 * Priority constant for the println method; use Log.v.
 */
const int XLM_VERBOSE = 2;

/**
 * Priority constant for the println method; use Log.d.
 */
const int XLM_DEBUG = 3;

/**
 * Priority constant for the println method; use Log.i.
 */
const int XLM_INFO = 4;

/**
 * Priority constant for the println method; use Log.w.
 */
const int XLM_WARN = 5;

/**
 * Priority constant for the println method; use Log.e.
 */
const int XLM_ERROR = 6;
/**
 * Priority constant for the println method; use assert.
 */
const int XLM_FATAL = 7;


int native_print(int level,int type, const char *tag,const char *file, int line, const char* fmt, ...) __attribute__((format(printf, 6, 7)));

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#define LOG_WRITE_FILE//即使写文件，没有初始化，一样没有日志文件


#ifndef LOG_WRITE_FILE

#if defined(__ANDROID__)

#include <android/log.h>
#define LOG_NDEBUG 0
#ifdef LOG_NDEBUG
    #if !LOG_NDEBUG
    #ifndef LOG_TAG
    #define LOG_TAG "qmediasdk"
    #endif
    #define  LOGV(...) //__android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
    #define  LOGD(...) //__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
    #define  LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
    #define  LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
    #define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
    #define  LOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
    #else
    #define  LOGV(...)
    #define  LOGD(...)
    #define  LOGI(...)
    #define  LOGW(...)
    #define  LOGE(...)
    #define  LOGF(...)
    #endif
#else
 #define  LOGV(...)
 #define  LOGD(...)
 #define  LOGI(...)
 #define  LOGW(...)
 #define  LOGE(...)
 #define  LOGF(...)
#endif

#else

#define GettidA syscall(SYS_gettid)
//#define gettid() syscall(__NR_gettid)

#define  LOGV(...)
#define  LOGD(...)

#define  LOGI(...) native_print(0, "qmediasdk", __VA_ARGS__)


#define  LOGW(...)
#define  LOGE(...) native_print(0, "qmediasdk", __VA_ARGS__)
#define  LOGF(...)

#endif

#else//LOG_WRITE_FILE

#define  LOGF(...) native_print(XLM_FATAL,1, "qmediasdk",__FILENAME__,__LINE__, __VA_ARGS__)
#define  LOGV(...) native_print(XLM_VERBOSE,1, "qmediasdk",__FILENAME__,__LINE__, __VA_ARGS__)
#define  LOGD(...) native_print(XLM_DEBUG,1, "qmediasdk",__FILENAME__,__LINE__, __VA_ARGS__)
#define  LOGI(...) native_print(XLM_INFO,1, "qmediasdk",__FILENAME__,__LINE__, __VA_ARGS__)
#define  LOGW(...) native_print(XLM_WARN,1, "qmediasdk",__FILENAME__,__LINE__, __VA_ARGS__)
#define  LOGE(...) native_print(XLM_ERROR,1, "qmediasdk",__FILENAME__,__LINE__, __VA_ARGS__)

#endif

#endif /* Logger_hpp */
