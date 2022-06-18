// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_session.djinni

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace QMedia { namespace Api {

class AudioRender;
class AudioRunloop;
class MediaSegment;
class VideoRender;
class VideoRunloop;
struct MediaRange;
struct Size;
struct Vec4f;

class MediaSessionInternal {
public:
    virtual ~MediaSessionInternal() {}

    virtual std::shared_ptr<MediaSegment> cresteMediaSegment(const std::string & filename, int32_t flag) = 0;

    virtual bool addMediaSegment(const std::shared_ptr<MediaSegment> & segment) = 0;

    virtual bool removeMediaSegment(const std::shared_ptr<MediaSegment> & segment) = 0;

    virtual std::vector<std::shared_ptr<MediaSegment>> getSegments() = 0;

    virtual MediaRange getTotalTimeRange() = 0;

    virtual std::shared_ptr<VideoRender> getVideoRender() = 0;

    virtual std::shared_ptr<AudioRender> getAudioRender() = 0;

    virtual void setAudioRunLoop(const std::shared_ptr<AudioRunloop> & audio_loop) = 0;

    virtual void setVideoRunLoop(const std::shared_ptr<VideoRunloop> & video_loop) = 0;

    virtual void setDisplayLayerSize(const Size & size) = 0;

    virtual Size getDisplayLayerSize() = 0;

    virtual void setBkColor(const Vec4f & color) = 0;

    virtual Vec4f getBkColor() = 0;

    /** control */
    virtual void prepare() = 0;

    virtual void start() = 0;

    virtual void stop() = 0;
};

} }  // namespace QMedia::Api
