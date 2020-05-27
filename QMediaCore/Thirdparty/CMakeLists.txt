cmake_minimum_required(VERSION 3.6)
#project(thirdparty)

option(BUILD_EXT_FFMPEG "Build with internal ffmpeg support" OFF)

if(BUILD_EXT_FFMPEG)
    add_subdirectory(ffmpeg)
    target_link_libraries(thirdparty ffmpeg)
endif(BUILD_EXT_FFMPEG)

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
