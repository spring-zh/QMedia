// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#include "media_range.h"  // my header

namespace QMedia { namespace Api {


bool operator==(const MediaRange& lhs, const MediaRange& rhs) {
    return lhs.start == rhs.start &&
           lhs.end == rhs.end;
}

bool operator!=(const MediaRange& lhs, const MediaRange& rhs) {
    return !(lhs == rhs);
}

} }  // namespace QMedia::Api