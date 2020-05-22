//
// Created by spring on 2020/5/21.
//

#ifndef QMEDIASDK_QEDITORPLAYER_JNI_H
#define QMEDIASDK_QEDITORPLAYER_JNI_H

#include "JniUtils.h"
#include "EffectEditor/EditorPlayer.h"

class QEditorPlayer_Jni : public EditorPlayer , EditorPlayer::Callback{
public:

    QEditorPlayer_Jni(jobject jpalyer);
    ~QEditorPlayer_Jni();

    void onPrepare(RetCode code) override;

    void onPlayerStateChanged(PlayerState newState, PlayerState oldState) override;

    void onProgressUpdated(int64_t mSec) override;

    void onSeekTo(int64_t mSec) override;

    void onCompleted() override;

private:

    jobject _object;
};


#endif //QMEDIASDK_QEDITORPLAYER_JNI_H
