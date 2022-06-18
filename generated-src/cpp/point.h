// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#pragma once

#include <cstdint>
#include <utility>

namespace QMedia { namespace Api {

struct Point final {
    int64_t x;
    int64_t y;

    friend bool operator==(const Point& lhs, const Point& rhs);
    friend bool operator!=(const Point& lhs, const Point& rhs);

    Point() {}

    Point(int64_t x_,
          int64_t y_)
    : x(std::move(x_))
    , y(std::move(y_))
    {}
};

} }  // namespace QMedia::Api
