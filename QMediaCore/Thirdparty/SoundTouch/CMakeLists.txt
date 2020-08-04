cmake_minimum_required(VERSION 3.6)

set(SoundTouch_HEADER
        SoundTouch/soundtouch_config.h
        SoundTouch/SoundTouch.h
        SoundTouch/TDStretch.h
        SoundTouch/AAFilter.h
        SoundTouch/FIRFilter.h
        SoundTouch/RateTransposer.h
        SoundTouch/FIFOSampleBuffer.h
        SoundTouch/InterpolateShannon.h
        SoundTouch/BPMDetect.h
        SoundTouch/PeakFinder.h
)

set(SoundTouch_SRC

        SoundTouch/SoundTouch.cpp
        SoundTouch/TDStretch.cpp
        SoundTouch/AAFilter.cpp
        SoundTouch/FIRFilter.cpp
        SoundTouch/RateTransposer.cpp
        SoundTouch/FIFOSampleBuffer.cpp
        SoundTouch/InterpolateShannon.cpp
        SoundTouch/InterpolateCubic.cpp
        SoundTouch/InterpolateLinear.cpp
        SoundTouch/BPMDetect.cpp
        SoundTouch/PeakFinder.cpp
        SoundTouch/sse_optimized.cpp
        SoundTouch/mmx_optimized.cpp
        SoundTouch/cpu_detect_x86.cpp
)
