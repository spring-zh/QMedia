//
// Created by spring on 2020/5/29.
//

#ifndef QMEDIASDK_QEXPORTER_JNI_H
#define QMEDIASDK_QEXPORTER_JNI_H

#include "JniUtils.h"
#include "EffectEditor/EditorExporter.h"

class QExporter_Jni : public EditorExporter , public EditorExporter::Callback{
public:
    QExporter_Jni(jobject jexporter);
    ~QExporter_Jni();

    void onStarted(RetCode code) override;

    void onStoped(RetCode code) override;

    void onProgressUpdated(int64_t mSec) override;

    void onCanceled(RetCode code) override;

    void onCompleted() override;

private:

    JniUtils::JWeakObject _weakObj;
};


#endif //QMEDIASDK_QEXPORTER_JNI_H
