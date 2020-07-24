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

        Thirdparty/SoundTouch/soundtouch_config.h
        Thirdparty/SoundTouch/SoundTouch.h
        Thirdparty/SoundTouch/TDStretch.h
        Thirdparty/SoundTouch/AAFilter.h
        Thirdparty/SoundTouch/FIRFilter.h
        Thirdparty/SoundTouch/RateTransposer.h
        Thirdparty/SoundTouch/FIFOSampleBuffer.h
        Thirdparty/SoundTouch/InterpolateShannon.h
        Thirdparty/SoundTouch/BPMDetect.h
        Thirdparty/SoundTouch/PeakFinder.h
)

set(THIRDPARTY_SRC

        Thirdparty/speex_resampler/speex_resampler.c
        Thirdparty/SoundTouch/SoundTouch.cpp
        Thirdparty/SoundTouch/TDStretch.cpp
        Thirdparty/SoundTouch/AAFilter.cpp
        Thirdparty/SoundTouch/FIRFilter.cpp
        Thirdparty/SoundTouch/RateTransposer.cpp
        Thirdparty/SoundTouch/FIFOSampleBuffer.cpp
        Thirdparty/SoundTouch/InterpolateShannon.cpp
        Thirdparty/SoundTouch/InterpolateCubic.cpp
        Thirdparty/SoundTouch/InterpolateLinear.cpp
        Thirdparty/SoundTouch/BPMDetect.cpp
        Thirdparty/SoundTouch/PeakFinder.cpp
        Thirdparty/SoundTouch/sse_optimized.cpp
        Thirdparty/SoundTouch/mmx_optimized.cpp
        Thirdparty/SoundTouch/cpu_detect_x86.cpp
)
