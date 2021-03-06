/*
 * Copyright (C) 2015 Zhang Rui <bbcallen@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * https://github.com/Bilibili/jni4android
 * This file is automatically generated by jni4android, do not modify.
 */

#include "QVideoRender.h"

typedef struct J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender {
    jclass id;

    jmethodID method_onVideoRender;
    jmethodID method_onVideoCreate;
    jmethodID method_onVideoDestroy;
} J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender;
static J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender;

jboolean J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender__onVideoRender(JNIEnv *env, jobject thiz, jlong wantTime)
{
    return (*env)->CallBooleanMethod(env, thiz, class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.method_onVideoRender, wantTime);
}

jboolean J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender__onVideoRender__catchAll(JNIEnv *env, jobject thiz, jlong wantTime)
{
    jboolean ret_value = J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender__onVideoRender(env, thiz, wantTime);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return false;
    }

    return ret_value;
}

jboolean J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender__onVideoCreate(JNIEnv *env, jobject thiz)
{
    return (*env)->CallBooleanMethod(env, thiz, class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.method_onVideoCreate);
}

jboolean J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender__onVideoCreate__catchAll(JNIEnv *env, jobject thiz)
{
    jboolean ret_value = J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender__onVideoCreate(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return false;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender__onVideoDestroy(JNIEnv *env, jobject thiz)
{
    (*env)->CallVoidMethod(env, thiz, class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.method_onVideoDestroy);
}

void J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender__onVideoDestroy__catchAll(JNIEnv *env, jobject thiz)
{
    J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender__onVideoDestroy(env, thiz);
    J4A_ExceptionCheck__catchAll(env);
}

int J4A_loadClass__J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender(JNIEnv *env)
{
    int         ret                   = -1;
    const char *J4A_UNUSED(name)      = NULL;
    const char *J4A_UNUSED(sign)      = NULL;
    jclass      J4A_UNUSED(class_id)  = NULL;
    int         J4A_UNUSED(api_level) = 0;

    if (class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.id != NULL)
        return 0;

    sign = "com/qmedia/qmediasdk/QTarget/QVideoRender";
    class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.id = J4A_FindClass__asGlobalRef__catchAll(env, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.id == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.id;
    name     = "onVideoRender";
    sign     = "(J)Z";
    class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.method_onVideoRender = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.method_onVideoRender == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.id;
    name     = "onVideoCreate";
    sign     = "()Z";
    class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.method_onVideoCreate = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.method_onVideoCreate == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.id;
    name     = "onVideoDestroy";
    sign     = "()V";
    class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.method_onVideoDestroy = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QTarget_QVideoRender.method_onVideoDestroy == NULL)
        goto fail;

    J4A_ALOGD("J4ALoader: OK: '%s' loaded\n", "com.qmedia.qmediasdk.QTarget.QVideoRender");
    ret = 0;
fail:
    return ret;
}
