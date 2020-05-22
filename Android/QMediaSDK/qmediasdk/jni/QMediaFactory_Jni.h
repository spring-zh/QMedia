//
// Created by spring on 2020/5/21.
//

#ifndef QMEDIASDK_QMEDIAFACTORY_JNI_H
#define QMEDIASDK_QMEDIAFACTORY_JNI_H

#include "VideoTargetAdapter.h"
#include "AudioTargetAdapter.h"

class QMediaFactory_Jni {
public:
    QMediaFactory_Jni() = default;
    ~QMediaFactory_Jni() = default;

    std::shared_ptr<VideoTargetAdapter> _videoTargetAdapter;
    std::shared_ptr<AudioTargetAdapter> _audioTargetAdapter;
};


#endif //QMEDIASDK_QMEDIAFACTORY_JNI_H
