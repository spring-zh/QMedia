set(MEDIA_CORE_HEADER
        MediaCore/audiocore/AudioClock.h
        MediaCore/audiocore/AudioProcess.h
        MediaCore/audiocore/ResamplerSpeex.h
        MediaCore/audiocore/AudioEffect.h

        MediaCore/core/DecodedFrameQueue.h
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
        MediaCore/demuxer/Mp4Demuxer.h

        MediaCore/output/OutputTarget.h
        MediaCore/output/AudioTarget.h
        MediaCore/output/VideoTarget.h

        MediaCore/protocol/adts.h
        MediaCore/protocol/golomb.h
        MediaCore/protocol/h264_parse.h
        MediaCore/protocol/hevc_parse.h
        MediaCore/protocol/sdp.h
        MediaCore/protocol/fs_io.h

        MediaCore/QPlayer.h
    )

set(MEDIA_CORE_SRC
        MediaCore/audiocore/AudioProcess.cpp
        MediaCore/audiocore/ResamplerSpeex.cpp
        MediaCore/audiocore/AudioEffect.cpp

        MediaCore/core/AudioFrame.cpp
        MediaCore/core/VideoFrame.cpp
        MediaCore/core/MediaStream.cpp

        MediaCore/decoder/SoftwareDecoder.cpp

        MediaCore/demuxer/GeneralDemuxer.cpp
        MediaCore/demuxer/Mp4Demuxer.cpp

        MediaCore/protocol/h264_parse.c
        MediaCore/protocol/hevc_parse.c
        MediaCore/protocol/sdp_decode.c
        MediaCore/protocol/sdp_util.c
        MediaCore/protocol/fs_io.c

        MediaCore/QPlayer.cpp
    )
