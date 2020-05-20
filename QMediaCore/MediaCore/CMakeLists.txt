set(MEDIA_CORE_HEADER

        MediaCore/core/AudioProcess.h
        MediaCore/core/DecodedFrameQueue.h
        MediaCore/core/ResamplerSpeex.h
        MediaCore/core/SteadyClock.h

        MediaCore/decoder/AudioDecoder.h
        MediaCore/decoder/VideoDecoder.h

        MediaCore/mediadef/AudioFrame.h
        MediaCore/mediadef/EncodedPacket.h
        MediaCore/mediadef/CodecSpecificInfo.h
        MediaCore/mediadef/MediaComm.h
        MediaCore/mediadef/VideoFrame.h

        MediaCore/output/OutputTarget.h
        MediaCore/output/AudioTarget.h
        MediaCore/output/VideoTarget.h

        MediaCore/AudioClock.h
        MediaCore/QPlayer.h
    )

set(MEDIA_CORE_SRC

        MediaCore/core/AudioProcess.cpp
        MediaCore/core/ResamplerSpeex.cpp

        MediaCore/mediadef/AudioFrame.cpp
        MediaCore/mediadef/VideoFrame.cpp

        MediaCore/QPlayer.cpp
    )
