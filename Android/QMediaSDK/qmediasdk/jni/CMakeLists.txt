# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# set(OpenCV_DIR /Users/ts/Downloads/OpenCV-android-sdk/sdk/native/jni)
# find_package(OpenCV REQUIRED)

#thirdparty module
#ffmpeg
# set(FFMPEG_PATH /Users/spring/Desktop/ffmpeg-build/ffmpeg-3.2.4/android)
# add_definitions(-D__STDC_CONSTANT_MACROS)
# include_directories( ${FFMPEG_PATH}/${ANDROID_ABI}/include )
# link_directories( ${FFMPEG_PATH}/${ANDROID_ABI}/lib )
set(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/j4a J4A_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/j4a_generate GENERATE_SRCS)

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

# link_directories( ${CORE_DIR}/Video/thirdparty/ffmpeg/lib/android )

if("${ANDROID_ABI}" STREQUAL "armeabi-v7a")
    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-warn-shared-textrel")
endif()
target_link_libraries( # Specifies the target library.
                       qmediasdk

                       qmediacore
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       # ${OpenCV_LIBS}
                       -lGLESv2
                       -llog 
                       )
