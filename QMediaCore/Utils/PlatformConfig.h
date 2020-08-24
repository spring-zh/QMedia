/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __BASE_PLATFORM_CONFIG_H__
#define __BASE_PLATFORM_CONFIG_H__
/// @cond DO_NOT_SHOW

/**
  Config of cocos2d-x project, per target platform.

  THIS FILE MUST NOT INCLUDE ANY OTHER FILE
*/

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which CC uses.
#define PLATFORM_UNKNOWN            0
#define PLATFORM_IOS                1
#define PLATFORM_ANDROID            2
#define PLATFORM_WIN32              3
#define PLATFORM_MARMALADE          4
#define PLATFORM_LINUX              5
#define PLATFORM_BADA               6
#define PLATFORM_BLACKBERRY         7
#define PLATFORM_MAC                8
#define PLATFORM_NACL               9
#define PLATFORM_EMSCRIPTEN        10
#define PLATFORM_TIZEN             11
#define PLATFORM_QT5               12
#define PLATFORM_WINRT             13

// Determine target platform by compile environment macro.
#define TARGET_PLATFORM             PLATFORM_UNKNOWN

// Apple: Mac and iOS
#if defined(__APPLE__) && !defined(ANDROID) // exclude android for binding generator.
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE // TARGET_OS_IPHONE includes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
        #undef  TARGET_PLATFORM
        #define TARGET_PLATFORM         PLATFORM_IOS
    #elif TARGET_OS_MAC
        #undef  TARGET_PLATFORM
        #define TARGET_PLATFORM         PLATFORM_MAC
    #endif
#endif

// android
#if defined(ANDROID)
    #undef  TARGET_PLATFORM
    #define TARGET_PLATFORM         PLATFORM_ANDROID
#endif

// win32
#if defined(_WIN32) && defined(_WINDOWS)
    #undef  TARGET_PLATFORM
    #define TARGET_PLATFORM         PLATFORM_WIN32
#endif

// linux
#if defined(LINUX) && !defined(__APPLE__)
    #undef  TARGET_PLATFORM
    #define TARGET_PLATFORM         PLATFORM_LINUX
#endif

// marmalade
#if defined(MARMALADE)
#undef  TARGET_PLATFORM
#define TARGET_PLATFORM         PLATFORM_MARMALADE
#endif

// bada
#if defined(SHP)
#undef  TARGET_PLATFORM
#define TARGET_PLATFORM         PLATFORM_BADA
#endif

// qnx
#if defined(__QNX__)
    #undef  TARGET_PLATFORM
    #define TARGET_PLATFORM     PLATFORM_BLACKBERRY
#endif

// native client
#if defined(__native_client__)
    #undef  TARGET_PLATFORM
    #define TARGET_PLATFORM     PLATFORM_NACL
#endif

// Emscripten
#if defined(EMSCRIPTEN)
    #undef  TARGET_PLATFORM
    #define TARGET_PLATFORM     PLATFORM_EMSCRIPTEN
#endif

// tizen
#if defined(TIZEN)
    #undef  TARGET_PLATFORM
    #define TARGET_PLATFORM     PLATFORM_TIZEN
#endif

// qt5
#if defined(TARGET_QT5)
    #undef  TARGET_PLATFORM
    #define TARGET_PLATFORM     PLATFORM_QT5
#endif

// WinRT (Windows 8.1 Store/Phone App)
#if defined(WINRT)
    #undef  TARGET_PLATFORM
    #define TARGET_PLATFORM          PLATFORM_WINRT
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if ! TARGET_PLATFORM
    #error  "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif

#if (TARGET_PLATFORM == PLATFORM_WIN32)
#ifndef __MINGW32__
#pragma warning (disable:4127)
#endif
#endif  // PLATFORM_WIN32

#if ((TARGET_PLATFORM == PLATFORM_ANDROID) || (TARGET_PLATFORM == PLATFORM_IOS) || (TARGET_PLATFORM == PLATFORM_TIZEN))
    #define PLATFORM_MOBILE
#else
    #define PLATFORM_PC
#endif


#if defined(__GNUC__) || defined(__clang__)
#    define q_builtin_constant_p __builtin_constant_p
#    define q_printf_format(fmtpos, attrpos) __attribute__((__format__(__printf__, fmtpos, attrpos)))
#else
#    define q_builtin_constant_p(x) 0
#    define q_printf_format(fmtpos, attrpos)
#endif

/// @endcond
#endif  // __BASE_PLATFORM_CONFIG_H__
