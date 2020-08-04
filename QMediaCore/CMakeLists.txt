# The version number
project(qmediacore)

set(QMEDIA_CORE_VERSION 1.0.0)

set(QMEDIA_CORE_LIB qmediacore)

set(THIRDPARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty)

include(EffectEditor/CMakeLists.txt)
include(GraphicCore/CMakeLists.txt)
include(MediaCore/CMakeLists.txt)
include(Utils/CMakeLists.txt)

add_subdirectory(Thirdparty)

set(QMEDIA_HEADER
    ${EFFECT_EDITOR_HEADER}
    ${GRAPHIC_CORE_HEADER}
    ${MEDIA_CORE_HEADER}
    ${UTILS_HEADER}
    )

set(QMEDIA_SRC
    ${EFFECT_EDITOR_SRC}
    ${GRAPHIC_CORE_SRC}
    ${MEDIA_CORE_SRC}
    ${UTILS_SRC}
    )

list(APPEND QMEDIA_SRC ${QMEDIA_HEADER})

add_library(qmediacore ${QMEDIA_SRC} ${THIRDPARTY_SRC})

if(ANDROID)
# CMake from android SDK set ANDROID_NDK instead of CMAKE_ANDROID_NDK
    include(AndroidNdkModules)
    android_ndk_import_module_cpufeatures()
#    if(ANDROID_NDK)
#        add_library(cpufeatures ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
#        target_include_directories(cpufeatures PUBLIC ${ANDROID_NDK}/sources/android/cpufeatures)
#    else()
#        add_library(cpufeatures ${CMAKE_ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
#        target_include_directories(cpufeatures PUBLIC ${CMAKE_ANDROID_NDK}/sources/android/cpufeatures)
#    endif()

    target_link_libraries(qmediacore cpufeatures)
endif()


#ffmpeg setting
if(ENABLE_FFMPEG)
    set(FFMPEG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/ffmpeg)
    set(FFMPEG_LIB_PATH ${FFMPEG_PATH}/lib/android/${ANDROID_ABI})

    target_include_directories(qmediacore
            PRIVATE ${FFMPEG_PATH}/include)
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

    target_link_libraries(qmediacore SoundTouch mp4 thirdparty avformat avcodec avutil swscale swresample z )
endif()

target_include_directories(qmediacore
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty
#    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Utils
#    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/GraphicCore
#    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/MediaCore
#    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/EffectEditor
)

set_target_properties(qmediacore
                      PROPERTIES
                      VERSION "${QMEDIA_CORE_VERSION}"
                      )


