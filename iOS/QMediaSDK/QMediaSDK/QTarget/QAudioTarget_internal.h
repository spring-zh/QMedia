//
//  QAudioTarget_internal.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QAudioTarget.h"
#include "MediaCore/output/AudioTarget.h"

extern QAudioDescribe* audioDescribeToXM(struct AudioDescribe desc);

#pragma mark QAudioTarget(object-c) to AudioTarget(c++)
class AudioTargetAdapter : public AudioTarget {
public:
    explicit AudioTargetAdapter(id<QAudioTarget> audioTarget):
    _audioTarget(audioTarget) {}
    ~AudioTargetAdapter() {}
    bool start() override { return [_audioTarget startAudio]; }
    bool stop() override { return [_audioTarget stopAudio]; }
    void pause(bool isPause) override {
        if (isPause) {
            [_audioTarget pauseAudio];
        }else
            [_audioTarget resumeAudio];
    }
    void flush() override { [_audioTarget flushAudio]; }
    
    //for audio
    bool init(AudioDescribe audioDesc) override {
        QAudioDescribe *xmAudioDesc = audioDescribeToXM(audioDesc);
        return [_audioTarget initAudio:xmAudioDesc];
    }
    void unInit() override{
        [_audioTarget unInitAudio];
    }
    int getSampleRate() const override {
        return [_audioTarget getSampleRate];
    }
    int getChannels() const override {
        return [_audioTarget getChannels];
    }
    RawAudioFormat getFormat() const override {
        return (RawAudioFormat)[_audioTarget getFormat];
    }
    
    void setVolume(float volume) override {
        [_audioTarget setVolume:volume];
    }
    float getVolume() const override {
        return [_audioTarget getVolume];
    }

    int getAudioDelayBytes() const override {
        return [_audioTarget audioDelay];
    }
    
    //
    bool onAudioDataRequest(void* audioData, int requestSize, int64_t wantTime){
        if(_render)
            return _render->onAudioRender((uint8_t *)audioData, requestSize, wantTime);
        else
            return false;
    }
    
private:
    __weak id<QAudioTarget> _audioTarget;
};


#pragma mark QAudioTarget is deprecated
@interface QAudioTarget : NSObject <QAudioRender>

-(instancetype)initWithTarget:(id<QAudioTarget>)target;
@property (nonatomic, readonly) id<QAudioTarget> ocTarget;
@property (nonatomic, readonly) AudioTarget* native;

@end
