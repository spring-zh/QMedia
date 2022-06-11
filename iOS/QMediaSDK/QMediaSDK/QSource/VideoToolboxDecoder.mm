//
//  VideoToolboxDecoder.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Utils/Logger.h"
#include "VideoToolboxDecoder.h"
//#include "avcc_parse.h"
//#include "hevc_parse.h"

#pragma mark --------- PixelFrameBuffer --------------
VTFrameBuffer::VTFrameBuffer(CVImageBufferRef _Nonnull imageBuffer, CMTime presentationTimeStamp){
//    _sampleBuffer = sampleBuffer;
    CFRetain(imageBuffer);
    _imageBuffer = imageBuffer;
    pixel_format_ = RawVideoFormat::kHardware;
    //get image info
    _width = (int) CVPixelBufferGetHeight(_imageBuffer);
    _heigth = (int) CVPixelBufferGetWidth(_imageBuffer);
    _data_size = (int)CVPixelBufferGetDataSize(_imageBuffer);
    _formatType = CVPixelBufferGetPixelFormatType(_imageBuffer);
    
    //get timestamp
//    CMTime currentSampleTime = CMSampleBufferGetOutputPresentationTimeStamp(sampleBuffer);
    _time_stamp = CMTimeGetSeconds(presentationTimeStamp) * kNumMillisecsPerSec;
}
VTFrameBuffer::~VTFrameBuffer(){
//    CMSampleBufferInvalidate(_sampleBuffer);
    CFRelease(_imageBuffer);
}

int VTFrameBuffer::Channels() const {
    return 0;
}

int VTFrameBuffer::SampleRate() const {
    return 0;
}

int VTFrameBuffer::SampleBits() const {
    return 0;
}

int VTFrameBuffer::Samples() const {
    return 0;
}

const uint8_t *VTFrameBuffer::Data() const {
    return nullptr;
}

uint32_t VTFrameBuffer::Size() const {
    return 0;
}

ChannelLayout VTFrameBuffer::ChannelLayout() const {
    return kMono;
}

int VTFrameBuffer::width() const {
    return _width;
}
int VTFrameBuffer::height() const {
    return _heigth;
}

const uint8_t* VTFrameBuffer::DataY() const {
    return (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(_imageBuffer,0);
}
const uint8_t* VTFrameBuffer::DataU() const {
    if (CVPixelBufferGetPlaneCount(_imageBuffer) >= 2) {
        return (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(_imageBuffer,1);
    }else
        return NULL;
}
const uint8_t* VTFrameBuffer::DataV() const {
    if (CVPixelBufferGetPlaneCount(_imageBuffer) >= 3) {
        return (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(_imageBuffer,2);
    }else
        return NULL;
}

int VTFrameBuffer::StrideY() const {
    return (int)CVPixelBufferGetBytesPerRowOfPlane(_imageBuffer,0);
}
int VTFrameBuffer::StrideU() const {
    if (CVPixelBufferGetPlaneCount(_imageBuffer) >= 2) {
        return (int)CVPixelBufferGetBytesPerRowOfPlane(_imageBuffer,1);
    }else
        return 0;
}
int VTFrameBuffer::StrideV() const {
    if (CVPixelBufferGetPlaneCount(_imageBuffer) >= 3) {
        return (int)CVPixelBufferGetBytesPerRowOfPlane(_imageBuffer,2);
    }else
        return 0;
}

void* VTFrameBuffer::native_handle() const {
    return _imageBuffer;
}

std::shared_ptr<VideoFrameBuffer> VTFrameBuffer::NativeToI420Buffer() {
    return nullptr;
}


#pragma mark --------- VideoToolboxDecoder --------------

#   define SDL_FOURCC(a, b, c, d) \
        (((uint32_t)a) | (((uint32_t)b) << 8) | (((uint32_t)c) << 16) | (((uint32_t)d) << 24))
#define VTB_FCC_AVCC   SDL_FOURCC('C', 'c', 'v', 'a')

typedef struct {
    int nalu_type;
    uint8_t* ptr;
    int size;
    int nalu_header_length;
} NaluUnit;

int ParseHEVCNALUnitType(const uint8_t* buff) {
  return (int) ((buff[0] >> 1) & 0x3f);
}

int ParseNALUnitType(const uint8_t* buff, bool is_h265) {
    if (is_h265) {
        return ((buff[0] >> 1) & 0x3f);
    } else {
        return (buff[0] & 0x1f);
    }
}

static uint8_t* avc_find_startcode(const uint8_t* inp, int size, int* nal_header_size)
{
    uint8_t* p = (uint8_t*)inp;
    const uint8_t *end = p + size;
    uint8_t *retp = NULL;
    *nal_header_size = 3;
    uint32_t complare = 0xffff0000 | ((uint32_t)p[0] << 8) | ((uint32_t)p[1]);
    p += 2;
    
    while (p < end)
    {
        complare = (complare << 8) | ((uint32_t)p[0]);
        p++;
        if ((complare & 0x00ffffff) == 0x1)
        {
            if((complare & 0xff000000) == 0)
                *nal_header_size = 4;

            retp = p - *nal_header_size;
            break;
        }
    }

    return retp;
}

std::vector<NaluUnit> ParseNaluUnits(const uint8_t* data, int size, bool is_h265, bool is_avcc) {
    std::vector<NaluUnit> result;
    if (is_avcc) {
        uint8_t* ptr = (uint8_t*)data;
        int nalu_size = MEM_RB32(ptr);
        int nalu_size_total = nalu_size + 4;
        int offset = 0;
        while (nalu_size > 1 && offset + nalu_size_total <= size) {
            result.push_back({ParseNALUnitType(ptr + 4, is_h265), ptr, nalu_size, 4});
            offset += nalu_size_total;
            ptr += nalu_size_total;
            nalu_size = MEM_RB32(ptr);
            nalu_size_total = nalu_size + 4;
        }
    } else {
        //parse annexb
        const uint8_t *endp = data + size;
        int find_nalu_header_size;
        int new_nalu_header_size;
        uint8_t *find_ptr = avc_find_startcode(data, size, &new_nalu_header_size);
        while (find_ptr) {
            find_nalu_header_size = new_nalu_header_size;
            uint8_t *startp = find_ptr + find_nalu_header_size + 1;
            uint8_t *p = avc_find_startcode(startp, endp - startp, &new_nalu_header_size);
            int nalu_len = p ? (p - find_ptr) : (endp - find_ptr);
//            HandleSingleNaluUnit(codec_configs, slicebuffer, find_ptr + find_nalu_header_size, nalu_len - find_nalu_header_size, has_keyframe_);
            result.push_back({ParseNALUnitType(find_ptr + find_nalu_header_size, is_h265), find_ptr, nalu_len, find_nalu_header_size});
            find_ptr = p;
        }
    }
    return result;
}

static void dict_set_i32(CFMutableDictionaryRef dict, CFStringRef key,
                         int32_t value)
{
    CFNumberRef number;
    number = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
    CFDictionarySetValue(dict, key, number);
    CFRelease(number);
}

static void dict_set_string(CFMutableDictionaryRef dict, CFStringRef key, const char * value)
{
    CFStringRef string;
    string = CFStringCreateWithCString(NULL, value, kCFStringEncodingASCII);
    CFDictionarySetValue(dict, key, string);
    CFRelease(string);
}

static void dict_set_boolean(CFMutableDictionaryRef dict, CFStringRef key, bool value)
{
    CFDictionarySetValue(dict, key, value ? kCFBooleanTrue: kCFBooleanFalse);
}

static void dict_set_object(CFMutableDictionaryRef dict, CFStringRef key, CFTypeRef *value)
{
    CFDictionarySetValue(dict, key, value);
}

static void dict_set_data(CFMutableDictionaryRef dict, CFStringRef key, uint8_t * value, uint64_t length)
{
    CFDataRef data;
    data = CFDataCreate(NULL, value, (CFIndex)length);
    CFDictionarySetValue(dict, key, data);
    CFRelease(data);
}

static CMFormatDescriptionRef CreateFormatDescriptionFromCodecData(CMVideoCodecType format_id, int width, int height, const uint8_t *extradata, int extradata_size, uint32_t atom)
{
    CMFormatDescriptionRef fmt_desc = NULL;
    OSStatus status;

    CFMutableDictionaryRef par = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks);
    CFMutableDictionaryRef atoms = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks);
    CFMutableDictionaryRef extensions = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

    /* CVPixelAspectRatio dict */
    dict_set_i32(par, CFSTR ("HorizontalSpacing"), 0);
    dict_set_i32(par, CFSTR ("VerticalSpacing"), 0);
    
    /* SampleDescriptionExtensionAtoms dict */
    switch (format_id) {
        case kCMVideoCodecType_H264:
            dict_set_data(atoms, CFSTR ("avcC"), (uint8_t *)extradata, extradata_size);
            break;
        case kCMVideoCodecType_HEVC:
            dict_set_data(atoms, CFSTR ("hvcC"), (uint8_t *)extradata, extradata_size);
            break;
        default:
            break;
    }


      /* Extensions dict */
    dict_set_string(extensions, CFSTR ("CVImageBufferChromaLocationBottomField"), "left");
    dict_set_string(extensions, CFSTR ("CVImageBufferChromaLocationTopField"), "left");
    dict_set_boolean(extensions, CFSTR("FullRangeVideo"), FALSE);
    dict_set_object(extensions, CFSTR ("CVPixelAspectRatio"), (CFTypeRef *) par);
    dict_set_object(extensions, CFSTR ("SampleDescriptionExtensionAtoms"), (CFTypeRef *) atoms);
    status = CMVideoFormatDescriptionCreate(NULL, format_id, width, height, extensions, &fmt_desc);

    CFRelease(extensions);
    CFRelease(atoms);
    CFRelease(par);

    if (status == 0)
        return fmt_desc;
    else
        return NULL;
}

static CMSampleBufferRef CreateSampleBufferFrom(CMFormatDescriptionRef fmt_desc, CMSampleTimingInfo* timingInfo, void *demux_buff, size_t demux_size)
{
    OSStatus status;
    CMBlockBufferRef newBBufOut = NULL;
    CMSampleBufferRef sBufOut = NULL;

    status = CMBlockBufferCreateWithMemoryBlock(
                                                NULL,
                                                demux_buff,
                                                demux_size,
                                                kCFAllocatorNull,
                                                NULL,
                                                0,
                                                demux_size,
                                                FALSE,
                                                &newBBufOut);

    if (!status) {
        status = CMSampleBufferCreate(
                                      NULL,
                                      newBBufOut,
                                      TRUE,
                                      0,
                                      0,
                                      fmt_desc,
                                      1,
                                      1,
                                      timingInfo,
                                      0,
                                      NULL,
                                      &sBufOut);
    }

    if (newBBufOut)
        CFRelease(newBBufOut);
    if (status == 0) {
        return sBufOut;
    } else {
        return NULL;
    }
}


static void CFDictionarySetSInt32(CFMutableDictionaryRef dictionary, CFStringRef key, SInt32 numberSInt32)
{
    CFNumberRef number;
    number = CFNumberCreate(NULL, kCFNumberSInt32Type, &numberSInt32);
    CFDictionarySetValue(dictionary, key, number);
    CFRelease(number);
}

static void CFDictionarySetBoolean(CFMutableDictionaryRef dictionary, CFStringRef key, bool value)
{
    CFDictionarySetValue(dictionary, key, value ? kCFBooleanTrue : kCFBooleanFalse);
}

VideoToolboxDecoder::VideoToolboxDecoder() {
    
}

int32_t VideoToolboxDecoder::InitDecode(const MediaDescribe& media_describe,
                           int32_t number_of_cores, const CodecSpecificInfo& codec_specific_info) {
    auto &video_desc = media_describe._videodesc;
    is_avcc_ = codec_specific_info.GetCodecFlag() == 1;
    if (format_desc_) {
        CFRelease(format_desc_);
    }
    format_desc_ = CreateVideoFormatDescriptionRef(video_desc, codec_specific_info);
    
    session_ = CreateVTbSession(kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange, video_desc.width, video_desc.height);
    
    return session_ == NULL ? -1 : 0;
}

int32_t VideoToolboxDecoder::Decode(const EncodedPacket &input_packet) {
    
    std::vector<NaluUnit> nalus = ParseNaluUnits(input_packet.getEncodedBuffer()->data(), input_packet.getEncodedBuffer()->size(), is_hevc_, is_avcc_);
    if (nalus.empty()) {
        return -2;
    }
    
    auto pts = input_packet.ntp_time_ms();
    auto dts = input_packet.getEncodedBuffer()->dts();
    
    CMSampleTimingInfo timingInfo = {
        .duration = CMTimeMake(1, 1000),
        .presentationTimeStamp = CMTimeMake(pts, 1000),
        .decodeTimeStamp = CMTimeMake(dts, 1000),
    };
    
    CMSampleBufferRef sample_buff;
    if (is_avcc_) {
        sample_buff = CreateSampleBufferFrom(format_desc_, &timingInfo, (void*)input_packet.getEncodedBuffer()->data(), input_packet.getEncodedBuffer()->size());
    } else {
        // find first slice nalu
        
        int sample_buff_size = 0;
        for (int i = 0 ; i < nalus.size(); ++i) {
            sample_buff_size += nalus[i].size - nalus[i].nalu_header_length + 4;
        }
        std::vector<uint8_t> sample_buff_temp(sample_buff_size);
        uint8_t* ptr = sample_buff_temp.data();
        for (int i = 0 ; i < nalus.size(); ++i) {
            int nalu_size = nalus[i].size - nalus[i].nalu_header_length;
            MEM_WB32(ptr, nalu_size);
            ptr += 4;
            memcpy(ptr, nalus[i].ptr + nalus[i].nalu_header_length, nalu_size);
            ptr += nalu_size;
        }
        
        sample_buff = CreateSampleBufferFrom(format_desc_, &timingInfo, (void*)sample_buff_temp.data(), sample_buff_temp.size());
    }
    
    VTDecodeFrameFlags flags = 0;  // kVTDecodeFrame_EnableTemporalProcessing;
    if (input_packet.flags() & EncodedPacket::FLAG_DISCARD) {
        flags |= kVTDecodeFrame_DoNotOutputFrame;
    }
    bool async = true;
    if (async) {
      flags |= kVTDecodeFrame_EnableAsynchronousDecompression;
    }
    OSStatus status = VTDecompressionSessionDecodeFrame(session_, sample_buff, flags, 0, nullptr);
    CFRelease(sample_buff);
    if (status != noErr) {
        if (status == kVTInvalidSessionErr) {
            //TODO: need restart
        }
    }
    
    return status == noErr ? 0 : -1;
}

int32_t VideoToolboxDecoder::Flush(bool wait_completed) {
    OSStatus status = VTDecompressionSessionFinishDelayedFrames(session_);
    if (wait_completed) {
        status = VTDecompressionSessionWaitForAsynchronousFrames(session_);
    }
    return (status == noErr) ? 0 : -1;
}

int32_t VideoToolboxDecoder::Release() {
    VTDecompressionSessionInvalidate(session_);
    CFRelease(session_);
    return 0;
}
 
void VideoToolboxDecoder::setOutputRequest(const MediaDescribe& mediaDescribe) {
    
}

CMVideoFormatDescriptionRef VideoToolboxDecoder::CreateVideoFormatDescriptionRef(const VideoDescribe& video_describe, const CodecSpecificInfo& codec_specific_info) {
    CMVideoFormatDescriptionRef format_desc;
    bool isHevcSupported = false;
    CMVideoCodecType format_id = 0;
    switch (video_describe.codectype) {
        case VideoCodecType::kH265:
            format_id = kCMVideoCodecType_HEVC;
            if (@available(iOS 11.0, *)) {
                isHevcSupported = VTIsHardwareDecodeSupported(kCMVideoCodecType_HEVC);
            } else {
                // Fallback on earlier versions
                isHevcSupported = false;
            }
            if (!isHevcSupported) {
                LOGE("isHevcSupported no !!!");
                return NULL;
            }
            break;
            
        case VideoCodecType::kH264:
            format_id = kCMVideoCodecType_H264;
            break;
            
        default:
            return NULL;
    }
    
    if (codec_specific_info.GetCodecFlag() == 1) {
        const std::vector<uint8_t >& extra_data = codec_specific_info.getExtraDatas()[0];
        format_desc = CreateFormatDescriptionFromCodecData(format_id, video_describe.width, video_describe.height, extra_data.data(), (int)extra_data.size(),  VTB_FCC_AVCC);
    } else {
        std::vector<size_t> param_set_sizes;
        std::vector<uint8_t*> param_set_ptrs;
        for (auto param : codec_specific_info.getExtraDatas()) {
            param_set_sizes.push_back(param.size());
            param_set_ptrs.push_back(param.data());
        }
        OSStatus status = -1;
        if (video_describe.codectype == VideoCodecType::kH265) {
            is_hevc_ = true;
            if (@available(iOS 11.0, *)) {
                status = CMVideoFormatDescriptionCreateFromHEVCParameterSets(
                                                                             NULL, param_set_ptrs.size(), param_set_ptrs.data(), param_set_sizes.data(), 4, NULL, &format_desc);
            } else {
                // Fallback on earlier versions
            }
        } else if (video_describe.codectype == VideoCodecType::kH264) {
            status = CMVideoFormatDescriptionCreateFromH264ParameterSets(
                                                                                  NULL, param_set_ptrs.size(), param_set_ptrs.data(), param_set_sizes.data(), 4, &format_desc);
        }
        
        if (status != noErr) {
            LOGE("CMVideoFormatDescriptionCreateFromParameterSets Failed !!");
            return NULL;
        }
    }
    
    return format_desc ;
}

VTDecompressionSessionRef VideoToolboxDecoder::CreateVTbSession(int pixelFormatType, int width, int height) {
    VTDecompressionSessionRef session;
    CFMutableDictionaryRef destinationPixelBufferAttributes = CFDictionaryCreateMutable(
                                                                 NULL,
                                                                 0,
                                                                 &kCFTypeDictionaryKeyCallBacks,
                                                                 &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetSInt32(destinationPixelBufferAttributes,
                          kCVPixelBufferPixelFormatTypeKey, pixelFormatType);
    if (width > 0 && height > 0) {
        CFDictionarySetSInt32(destinationPixelBufferAttributes,
                              kCVPixelBufferWidthKey, width);
        CFDictionarySetSInt32(destinationPixelBufferAttributes,
                              kCVPixelBufferHeightKey, height);
    }
    
    CFDictionarySetBoolean(destinationPixelBufferAttributes,
                          kCVPixelBufferOpenGLESCompatibilityKey, TRUE);
    
    VTDecompressionOutputCallbackRecord outputCallback = {&VideoToolboxDecoder::__DecompressionOutputCallback, this};
    OSStatus status = VTDecompressionSessionCreate(
        NULL, format_desc_ /*CMVideoFormatDescriptionRef  _Nonnull
                             videoFormatDescription*/
        ,
        NULL /*CFDictionaryRef  _Nullable videoDecoderSpecification*/,
        destinationPixelBufferAttributes /*CFDictionaryRef  _Nullable
                                 destinationImageBufferAttributes*/
        ,
        &outputCallback, &session);
    
    CFRelease(destinationPixelBufferAttributes);
    if (status != noErr) {
//        NSError* error = [NSError errorWithDomain:NSOSStatusErrorDomain code:status userInfo:nil];
//        NSLog(@"Error %@", [error description]);
        LOGE("error when VTDecompressionSessionCreate: %ld", (long)status);
    }
    
    return session;
}

void VideoToolboxDecoder::DecompressionOutputCallback( void* CM_NULLABLE sourceFrameRefCon,
                                                     OSStatus status,
                                                     VTDecodeInfoFlags infoFlags,
                                                     CM_NULLABLE CVImageBufferRef imageBuffer,
                                                     CMTime presentationTimeStamp,
                                                     CMTime presentationDuration) {
    if (imageBuffer) {
        //
        if (callback_) {
            auto framebuffer = std::shared_ptr<VTFrameBuffer>(new VTFrameBuffer(imageBuffer, presentationTimeStamp));
            DecodedFrame frame(framebuffer, CMTimeGetSeconds(presentationTimeStamp) * 1000);
            callback_->OnDecoded(frame);
        }
    }
}
void VideoToolboxDecoder::__DecompressionOutputCallback(void* CM_NULLABLE decompressionOutputRefCon,
                                                      void* CM_NULLABLE sourceFrameRefCon,
                                                      OSStatus status,
                                                      VTDecodeInfoFlags infoFlags,
                                                      CM_NULLABLE CVImageBufferRef imageBuffer,
                                                      CMTime presentationTimeStamp,
                                                      CMTime presentationDuration) {
    ((VideoToolboxDecoder*)decompressionOutputRefCon)->
    DecompressionOutputCallback(sourceFrameRefCon, status, infoFlags,
                                imageBuffer, presentationTimeStamp, presentationDuration);
}

#pragma mark ---------- HardwareDecoderFactory ----------

HardwareDecoderFactory* HardwareDecoderFactory::instance() {
    static HardwareDecoderFactory decoderFactory;
    return &decoderFactory;
}

HardwareDecoderFactory::HardwareDecoderFactory() {

}
HardwareDecoderFactory::~HardwareDecoderFactory() {

}

Decoder *HardwareDecoderFactory::CreateDecoder(const MediaDescribe& mediaDescribe) {
    return new VideoToolboxDecoder();
}

void HardwareDecoderFactory::DestroyDecoder(Decoder *decoder) {
    decoder->Release();
    delete decoder;
}
