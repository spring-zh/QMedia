# The version number
project(qmediacore)

#enable_language(ASM)

set(QMEDIA_CORE_VERSION 1.0.0)

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
    qmediacore.cpp
    ${EFFECT_EDITOR_SRC}
    ${GRAPHIC_CORE_SRC}
    ${MEDIA_CORE_SRC}
    ${UTILS_SRC}
    )

list(APPEND QMEDIA_SRC ${QMEDIA_HEADER})

add_library(qmediacore ${QMEDIA_SRC})

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

target_link_libraries(qmediacore thirdparty)

target_include_directories(qmediacore
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty
)

set_target_properties(qmediacore
                      PROPERTIES
                      VERSION "${QMEDIA_CORE_VERSION}"
                      )


