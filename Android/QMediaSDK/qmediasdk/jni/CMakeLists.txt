# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.


set(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")
option(ENABLE_FFMPEG "set ffmpeg enable" ON)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB J4A_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/j4a/*.c")
file(GLOB GENERATE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/j4a_generate/*.c")

set(MEDIACORE_DIR ../../../../QMediaCore)
add_subdirectory(${MEDIACORE_DIR} qmediacore)

add_library( # Sets the name of the library.
             qmediasdk
             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
             ${SRCS}
             ${J4A_SRCS}
             ${GENERATE_SRCS}
        )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

if(("${ANDROID_ABI}" STREQUAL "armeabi-v7a") OR ("${ANDROID_ABI}" STREQUAL "x86"))
    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-warn-shared-textrel")
endif()

target_link_libraries( # Specifies the target library.
                       qmediasdk
                       qmediacore
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       -lGLESv2
                       -lEGL
                       -llog 
                       )
