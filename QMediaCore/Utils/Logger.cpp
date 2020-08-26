//
//  Logger.cpp
//  QMediaCore
//
//  Created by LINQIUMING on 17/05/2017.
//  Copyright © 2019 QMedia. All rights reserved.
//

#include "Logger.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sys/time.h>

#if defined(__ANDROID__)

#include <android/log.h>

#endif


int global_log_level;

static int logIO = -1;
void initLogFile(const char *path)
{
    if (logIO != -1)
    {
        close(logIO);
    }

    logIO = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
}

void destroyLogFile()
{
    if (logIO != -1)
    {
        close(logIO);
    }
}


int native_print(int level,int type, const char *tag, const char *file,int line,const char* fmt, ...)
{
    if(level < global_log_level){
        return 0;
    }
#if defined(__ANDROID__)
    if(level > ANDROID_LOG_FATAL)
        return 0;
#endif
    std::stringstream ss;
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    timeval curTime;
    gettimeofday(&curTime, NULL);
    tm *time = std::localtime(&t);
    ss << (time->tm_mon + 1) << "-" << time->tm_mday << " "
       << time->tm_hour << ":" << time->tm_min << ":" << time->tm_sec << "."
       << curTime.tv_usec / 1000;


    char fmt2[2048] = {0};
    char logBuf[2048] = {0};
    memset(fmt2,0, sizeof(fmt2));
    snprintf(fmt2, 1024, "[%s]%s %s line:%d [T:%llu] %s \n", ss.str().c_str() ,tag, file,line, (unsigned long long)pthread_self(),fmt);
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(logBuf, 1024, fmt2, ap);
    va_end(ap);

    if (type == 0 || logIO == -1)
    {
        #if defined(__ANDROID__)
        
        __android_log_print(level, "qmediasdk", "%s", logBuf);

        #else
        
        fprintf(stdout, "%s", logBuf);

        #endif
        
    }
    else
    {
        write(logIO, logBuf, 1024);
    }
    
    return 0;
}
