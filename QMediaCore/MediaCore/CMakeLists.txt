set(MEDIA_CORE_HEADER

        MediaCore/core/AudioProcess.h
        MediaCore/core/DecodedFrameQueue.h
        MediaCore/core/ResamplerSpeex.h
        MediaCore/core/SteadyClock.h
        MediaCore/core/AudioFrame.h
        MediaCore/core/EncodedPacket.h
        MediaCore/core/CodecSpecificInfo.h
        MediaCore/core/MediaComm.h
        MediaCore/core/VideoFrame.h
        MediaCore/core/MediaStream.h

        MediaCore/decoder/AudioDecoder.h
        MediaCore/decoder/VideoDecoder.h
        MediaCore/decoder/Decoder.h
        MediaCore/decoder/SoftwareDecoder.h

        MediaCore/demuxer/Demuxer.h
        MediaCore/demuxer/GeneralDemuxer.h

        MediaCore/output/OutputTarget.h
        MediaCore/output/AudioTarget.h
        MediaCore/output/VideoTarget.h

        MediaCore/AudioClock.h
        MediaCore/QPlayer.h
    )

set(MEDIA_CORE_SRC

        MediaCore/core/AudioProcess.cpp
        MediaCore/core/ResamplerSpeex.cpp
        MediaCore/core/AudioFrame.cpp
        MediaCore/core/VideoFrame.cpp
        MediaCore/core/MediaStream.cpp

        MediaCore/decoder/SoftwareDecoder.cpp

        MediaCore/demuxer/GeneralDemuxer.cpp

        MediaCore/QPlayer.cpp
    )
