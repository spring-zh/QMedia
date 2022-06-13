// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#pragma once

#include <cstdint>
#include <utility>

namespace QMedia { namespace Api {

struct AudioDescription final {
    static const int16_t FORMAT_UNKNOW = -1;

    static const int16_t FORMAT_U8 = 0;

    static const int16_t FORMAT_S16 = 1;

    static const int16_t FORMAT_U16 = 2;

    static const int16_t FORMAT_S32 = 3;

    static const int16_t FORMAT_U32 = 4;

    static const int16_t FORMAT_FLOAT = 5;

    static const int16_t FORMAT_SAMPLEBUFFER = 100;

    static const int16_t FORMAT_BYTEARRAY = 101;

    int16_t smaple_format;
    int16_t nchannel;
    int32_t sample_rate;
    int32_t sample_bits;

    AudioDescription() {}

    AudioDescription(int16_t smaple_format_,
                     int16_t nchannel_,
                     int32_t sample_rate_,
                     int32_t sample_bits_)
    : smaple_format(std::move(smaple_format_))
    , nchannel(std::move(nchannel_))
    , sample_rate(std::move(sample_rate_))
    , sample_bits(std::move(sample_bits_))
    {}
};

} }  // namespace QMedia::Api
