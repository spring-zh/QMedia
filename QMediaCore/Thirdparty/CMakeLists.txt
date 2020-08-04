cmake_minimum_required(VERSION 3.6)
project(thirdparty)

include(SoundTouch/CMakeLists.txt)
#include(libmp4/CMakeLists.txt)
add_subdirectory(mp4)

set(THIRDPARTY_HEADER

        Thirdparty/stb/stb_image.h
        Thirdparty/stb/stb_image_write.h
        Thirdparty/speex_resampler/fixed_generic.h
        Thirdparty/speex_resampler/resample_neon.h
        Thirdparty/speex_resampler/speex_arch.h
        Thirdparty/speex_resampler/speex_resampler.h
        Thirdparty/speex_resampler/speex_stack_alloc.h
)

set(THIRDPARTY_SRC

        Thirdparty/speex_resampler/speex_resampler.c
)

add_library(SoundTouch ${SoundTouch_SRC} )
target_include_directories(SoundTouch
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SoundTouch/include)

#add_library(mp4 ${MP4_SRC} )
target_include_directories(mp4
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mp4)

add_library(thirdparty speex_resampler/speex_resampler.c )
target_include_directories(thirdparty
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/speex_resampler)

