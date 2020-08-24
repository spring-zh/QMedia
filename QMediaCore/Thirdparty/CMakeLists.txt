cmake_minimum_required(VERSION 3.6)
project(thirdparty)

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
        thirdparty.cpp
        speex_resampler/speex_resampler.c
        sonic/sonic.c
)

target_include_directories(mp4
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mp4)

add_library(thirdparty ${THIRDPARTY_SRC} )
target_include_directories(thirdparty
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/speex_resampler
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sonic)

#ffmpeg setting
if(ENABLE_FFMPEG)
    set(FFMPEG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg)
    set(FFMPEG_LIB_PATH ${FFMPEG_PATH}/lib/android/${ANDROID_ABI})

    target_include_directories(thirdparty
            PUBLIC ${FFMPEG_PATH}/include)
    add_definitions(-D__STDC_CONSTANT_MACROS)

    add_library(avformat STATIC IMPORTED)
    add_library(avcodec STATIC IMPORTED)
    add_library(avutil STATIC IMPORTED)
    add_library(swresample STATIC IMPORTED)
    add_library(swscale STATIC IMPORTED)

    set_target_properties( # Specifies the target library.
            avformat
            # Specifies the parameter you want to define.
            PROPERTIES IMPORTED_LOCATION
            # Provides the path to the library you want to import.
            ${FFMPEG_LIB_PATH}/libavformat.a)

    set_target_properties( avcodec      PROPERTIES IMPORTED_LOCATION  ${FFMPEG_LIB_PATH}/libavcodec.a)
    set_target_properties( swscale      PROPERTIES IMPORTED_LOCATION  ${FFMPEG_LIB_PATH}/libswscale.a)
    set_target_properties( swresample   PROPERTIES IMPORTED_LOCATION  ${FFMPEG_LIB_PATH}/libswresample.a)
    set_target_properties( avutil       PROPERTIES IMPORTED_LOCATION  ${FFMPEG_LIB_PATH}/libavutil.a)

    target_link_libraries(thirdparty avformat avcodec avutil swscale swresample z )
endif()

target_link_libraries(thirdparty mp4)

