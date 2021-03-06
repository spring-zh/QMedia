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

#include "QVideoDescribe.h"

typedef struct J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe {
    jclass id;

    jmethodID constructor_QVideoDescribe;
    jfieldID field_codec;
    jfieldID field_rawFormat;
    jfieldID field_rotation;
    jfieldID field_framerate;
    jfieldID field_width;
    jfieldID field_height;
    jfieldID field_bitrate;
    jfieldID field_timeScale;
} J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe;
static J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe;

jobject J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__QVideoDescribe(JNIEnv *env, jint codec, jint rawFormat, jint rotation, jfloat framerate, jint width, jint height, jint bitrate, jint timeScale)
{
    return (*env)->NewObject(env, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.constructor_QVideoDescribe, codec, rawFormat, rotation, framerate, width, height, bitrate, timeScale);
}

jobject J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__QVideoDescribe__catchAll(JNIEnv *env, jint codec, jint rawFormat, jint rotation, jfloat framerate, jint width, jint height, jint bitrate, jint timeScale)
{
    jobject ret_object = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__QVideoDescribe(env, codec, rawFormat, rotation, framerate, width, height, bitrate, timeScale);
    if (J4A_ExceptionCheck__catchAll(env) || !ret_object) {
        return NULL;
    }

    return ret_object;
}

jobject J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__QVideoDescribe__asGlobalRef__catchAll(JNIEnv *env, jint codec, jint rawFormat, jint rotation, jfloat framerate, jint width, jint height, jint bitrate, jint timeScale)
{
    jobject ret_object   = NULL;
    jobject local_object = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__QVideoDescribe__catchAll(env, codec, rawFormat, rotation, framerate, width, height, bitrate, timeScale);
    if (J4A_ExceptionCheck__catchAll(env) || !local_object) {
        ret_object = NULL;
        goto fail;
    }

    ret_object = J4A_NewGlobalRef__catchAll(env, local_object);
    if (!ret_object) {
        ret_object = NULL;
        goto fail;
    }

fail:
    J4A_DeleteLocalRef__p(env, &local_object);
    return ret_object;
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__codec__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_codec);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__codec__get__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__codec__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__codec__set(JNIEnv *env, jobject thiz, jint value)
{
    (*env)->SetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_codec, value);
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__codec__set__catchAll(JNIEnv *env, jobject thiz, jint value)
{
    J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__codec__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rawFormat__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_rawFormat);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rawFormat__get__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rawFormat__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rawFormat__set(JNIEnv *env, jobject thiz, jint value)
{
    (*env)->SetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_rawFormat, value);
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rawFormat__set__catchAll(JNIEnv *env, jobject thiz, jint value)
{
    J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rawFormat__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rotation__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_rotation);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rotation__get__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rotation__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rotation__set(JNIEnv *env, jobject thiz, jint value)
{
    (*env)->SetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_rotation, value);
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rotation__set__catchAll(JNIEnv *env, jobject thiz, jint value)
{
    J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rotation__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

jfloat J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__framerate__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetFloatField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_framerate);
}

jfloat J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__framerate__get__catchAll(JNIEnv *env, jobject thiz)
{
    jfloat ret_value = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__framerate__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__framerate__set(JNIEnv *env, jobject thiz, jfloat value)
{
    (*env)->SetFloatField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_framerate, value);
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__framerate__set__catchAll(JNIEnv *env, jobject thiz, jfloat value)
{
    J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__framerate__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__width__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_width);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__width__get__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__width__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__width__set(JNIEnv *env, jobject thiz, jint value)
{
    (*env)->SetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_width, value);
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__width__set__catchAll(JNIEnv *env, jobject thiz, jint value)
{
    J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__width__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__height__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_height);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__height__get__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__height__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__height__set(JNIEnv *env, jobject thiz, jint value)
{
    (*env)->SetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_height, value);
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__height__set__catchAll(JNIEnv *env, jobject thiz, jint value)
{
    J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__height__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__bitrate__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_bitrate);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__bitrate__get__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__bitrate__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__bitrate__set(JNIEnv *env, jobject thiz, jint value)
{
    (*env)->SetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_bitrate, value);
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__bitrate__set__catchAll(JNIEnv *env, jobject thiz, jint value)
{
    J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__bitrate__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__timeScale__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_timeScale);
}

jint J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__timeScale__get__catchAll(JNIEnv *env, jobject thiz)
{
    jint ret_value = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__timeScale__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__timeScale__set(JNIEnv *env, jobject thiz, jint value)
{
    (*env)->SetIntField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_timeScale, value);
}

void J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__timeScale__set__catchAll(JNIEnv *env, jobject thiz, jint value)
{
    J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__timeScale__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

int J4A_loadClass__J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe(JNIEnv *env)
{
    int         ret                   = -1;
    const char *J4A_UNUSED(name)      = NULL;
    const char *J4A_UNUSED(sign)      = NULL;
    jclass      J4A_UNUSED(class_id)  = NULL;
    int         J4A_UNUSED(api_level) = 0;

    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id != NULL)
        return 0;

    sign = "com/qmedia/qmediasdk/QSource/QVideoDescribe";
    class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id = J4A_FindClass__asGlobalRef__catchAll(env, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id;
    name     = "<init>";
    sign     = "(IIIFIIII)V";
    class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.constructor_QVideoDescribe = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.constructor_QVideoDescribe == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id;
    name     = "codec";
    sign     = "I";
    class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_codec = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_codec == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id;
    name     = "rawFormat";
    sign     = "I";
    class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_rawFormat = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_rawFormat == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id;
    name     = "rotation";
    sign     = "I";
    class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_rotation = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_rotation == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id;
    name     = "framerate";
    sign     = "F";
    class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_framerate = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_framerate == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id;
    name     = "width";
    sign     = "I";
    class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_width = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_width == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id;
    name     = "height";
    sign     = "I";
    class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_height = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_height == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id;
    name     = "bitrate";
    sign     = "I";
    class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_bitrate = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_bitrate == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.id;
    name     = "timeScale";
    sign     = "I";
    class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_timeScale = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe.field_timeScale == NULL)
        goto fail;

    J4A_ALOGD("J4ALoader: OK: '%s' loaded\n", "com.qmedia.qmediasdk.QSource.QVideoDescribe");
    ret = 0;
fail:
    return ret;
}
