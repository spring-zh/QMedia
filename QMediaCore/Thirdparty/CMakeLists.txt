cmake_minimum_required(VERSION 3.6)
project(thirdparty)

#include(libmp4/CMakeLists.txt)
add_subdirectory(mp4)

set(THIRDPARTY_HEADER

        stb/stb_image.h
        stb/stb_image_write.h
        speex_resampler/fixed_generic.h
        speex_resampler/resample_neon.h
        speex_resampler/speex_arch.h
        speex_resampler/speex_resampler.h
        speex_resampler/speex_stack_alloc.h

        sonic/sonic.h
)

set(THIRDPARTY_SRC

        speex_resampler/speex_resampler.c
        sonic/sonic.c
)

#add_library(mp4 ${MP4_SRC} )
target_include_directories(mp4
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mp4)

add_library(thirdparty ${THIRDPARTY_SRC} )
target_include_directories(thirdparty
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/speex_resampler
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sonic)

target_link_libraries(thirdparty mp4)

