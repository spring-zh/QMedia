//
// Created by spring on 2020/6/23.
//

#include "GraphicCore/effect/EffectManage.h"
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"

const char* QEffectConfig_ClassPath = "com/qmedia/qmediasdk/QEffect/QEffectConfig";

using GraphicCore::EffectManage;

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QEffect_QEffectManage_##name

NATIVE_FUNCTION(jobjectArray , native_1getAllEffectConfig)(JNIEnv *env, jobject thiz)
{
    auto allConfig = EffectManage::instance()->getAllEffectConfig();
    size_t lenght = allConfig.size();
    jclass effect_conifg_cls = env->FindClass(QEffectConfig_ClassPath);
    jobjectArray jconfig_array = env->NewObjectArray(lenght, effect_conifg_cls, NULL);
    for (int i = 0; i < allConfig.size(); ++i) {
        jstring jname = env->NewStringUTF(allConfig[i]->name.c_str());
        jstring jdescribe = env->NewStringUTF(allConfig[i]->describe.c_str());
        jstring jeffect_type = env->NewStringUTF(allConfig[i]->effect_type.c_str());

        jobject jconfig = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig(env, jname, jdescribe, jeffect_type);
        env->SetObjectArrayElement(jconfig_array, i, jconfig);
    }
    return jconfig_array;
}

NATIVE_FUNCTION(jboolean , native_1addFilterConfig)(JNIEnv *env, jobject thiz, jstring jconfig_json)
{
    const char *config_json = env->GetStringUTFChars(jconfig_json, NULL);
    std::string config_str = config_json;
    EffectManage::instance()->addFilterConfig(config_str);
    env->ReleaseStringUTFChars(jconfig_json, config_json);
    return 0;
}

NATIVE_FUNCTION(jlong , native_1createEffect)(JNIEnv *env, jobject thiz, jstring jeffect_name)
{
    const char *effect_name = env->GetStringUTFChars(jeffect_name, NULL);
    GraphicCore::Effect* effect = EffectManage::instance()->createEffect(effect_name);
    env->ReleaseStringUTFChars(jeffect_name, effect_name);

    if (effect) {
        return reinterpret_cast<jlong>(new GraphicCore::EffectRef(effect));
    } else
        return 0;
}