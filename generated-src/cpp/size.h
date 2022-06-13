// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#pragma once

#include <cstdint>
#include <utility>

namespace QMedia { namespace Api {

struct Size final {
    int64_t width;
    int64_t height;

    bool operator==(const Size& rhs) {
        return width == rhs.width &&
               height == rhs.height;
    }

    bool operator!=(const Size& rhs) {
        return !((*this) == rhs);
    }

    Size() {}

    Size(int64_t width_,
         int64_t height_)
    : width(std::move(width_))
    , height(std::move(height_))
    {}
};

} }  // namespace QMedia::Api
