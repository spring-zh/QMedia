// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#pragma once

#include <utility>

namespace QMedia { namespace Api {

struct Point final {
    float x;
    float y;

    friend bool operator==(const Point& lhs, const Point& rhs);
    friend bool operator!=(const Point& lhs, const Point& rhs);

    Point() {}

    Point(float x_,
          float y_)
    : x(std::move(x_))
    , y(std::move(y_))
    {}
};

} }  // namespace QMedia::Api
