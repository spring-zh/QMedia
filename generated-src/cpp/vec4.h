// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#pragma once

#include <utility>

namespace QMedia { namespace Api {

struct Vec4 final {
    float v1;
    float v2;
    float v3;
    float v4;

    bool operator==(const Vec4& rhs) {
        return v1 == rhs.v1 &&
               v2 == rhs.v2 &&
               v3 == rhs.v3 &&
               v4 == rhs.v4;
    }

    bool operator!=(const Vec4& rhs) {
        return !((*this) == rhs);
    }

    Vec4() {}

    Vec4(float v1_,
         float v2_,
         float v3_,
         float v4_)
    : v1(std::move(v1_))
    , v2(std::move(v2_))
    , v3(std::move(v3_))
    , v4(std::move(v4_))
    {}
};

} }  // namespace QMedia::Api
