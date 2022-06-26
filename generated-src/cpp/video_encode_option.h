// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_config.djinni

#pragma once

#include "Utils/optional.hpp"
#include <cstdint>
#include <string>
#include <utility>

namespace QMedia { namespace Api {

struct VideoEncodeOption final {
    /** codectype */
    static const int32_t KUNKNOWN;

    static const int32_t KVP8;

    static const int32_t KVP9;

    static const int32_t KMPEG4;

    static const int32_t KH264;

    static const int32_t KH265;

    static const int32_t KI420;

    /** profile */
    static const int32_t PROFILE_BASELINE;

    static const int32_t PROFILE_MAIN;

    static const int32_t PROFILE_HIGH;

    int32_t codectype;
    int32_t width;
    int32_t height;
    int32_t bitrate;
    int32_t framerate;
    int32_t gop;
    int32_t profile;
    int32_t level;
    std::experimental::optional<std::string> ext;

    VideoEncodeOption() {}

    VideoEncodeOption(int32_t codectype_,
                      int32_t width_,
                      int32_t height_,
                      int32_t bitrate_,
                      int32_t framerate_,
                      int32_t gop_,
                      int32_t profile_,
                      int32_t level_,
                      std::experimental::optional<std::string> ext_)
    : codectype(std::move(codectype_))
    , width(std::move(width_))
    , height(std::move(height_))
    , bitrate(std::move(bitrate_))
    , framerate(std::move(framerate_))
    , gop(std::move(gop_))
    , profile(std::move(profile_))
    , level(std::move(level_))
    , ext(std::move(ext_))
    {}
};

} }  // namespace QMedia::Api
