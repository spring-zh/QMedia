//
//  EditorExporter.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_EXPORTER_H
#define EFFECTEDITOR_EXPORTER_H
#if 0
#include "EffectCombiner.h"

class EditorExporter : public EffectCombiner, public EffectCombiner::Callback {
public:
    
    class Callback
    {
    public:
        virtual void onStarted(RetCode code) = 0;
        virtual void onStoped(RetCode code) = 0;
        virtual void onProgressUpdated(int64_t mSec) = 0;
        virtual void onCanceled(RetCode code) = 0;
        virtual void onCompleted() = 0;
    };
    
    EditorExporter();
    ~EditorExporter();

    void setCallBack(Callback *callback) { _callback = callback; }
    void start() override;
    void stop() override;
    void cancel();
    
    int64_t getPosition() const override;
    
protected:
    bool onVideoRender(int64_t wantTimeMs) override;
    bool onAudioRender(uint8_t *const buffer, unsigned int needBytes, int64_t wantTimeMs) override;
    
    //implement EffectEditorCombiner::Callback
    virtual void onPrepared(RetCode code) override;
    virtual void onStarted(RetCode code) override;
    virtual void onStoped(RetCode code) override;
    virtual void onStreamEnd(MediaType mediaType) override;
    virtual void onCompleted() override;
    
private:
    int64_t _renderPosition;
    RetCode _cancel();
    RetCode _complete();
    Callback *_callback;
    
    int64_t _lastRenderTime;
};
#endif
#endif /* EFFECTEDITOR_EXPORTER_H */
