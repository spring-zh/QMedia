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

#include "QRange.h"

typedef struct J4AC_com_qmedia_qmediasdk_QCommon_QRange {
    jclass id;

    jmethodID constructor_QRange;
    jmethodID method_isValid;
    jmethodID method_getLength;
    jfieldID field_start;
    jfieldID field_end;
} J4AC_com_qmedia_qmediasdk_QCommon_QRange;
static J4AC_com_qmedia_qmediasdk_QCommon_QRange class_J4AC_com_qmedia_qmediasdk_QCommon_QRange;

jobject J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange(JNIEnv *env, jlong start, jlong end)
{
    return (*env)->NewObject(env, class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.id, class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.constructor_QRange, start, end);
}

jobject J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange__catchAll(JNIEnv *env, jlong start, jlong end)
{
    jobject ret_object = J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange(env, start, end);
    if (J4A_ExceptionCheck__catchAll(env) || !ret_object) {
        return NULL;
    }

    return ret_object;
}

jobject J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange__asGlobalRef__catchAll(JNIEnv *env, jlong start, jlong end)
{
    jobject ret_object   = NULL;
    jobject local_object = J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange__catchAll(env, start, end);
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

jboolean J4AC_com_qmedia_qmediasdk_QCommon_QRange__isValid(JNIEnv *env, jobject thiz)
{
    return (*env)->CallBooleanMethod(env, thiz, class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.method_isValid);
}

jboolean J4AC_com_qmedia_qmediasdk_QCommon_QRange__isValid__catchAll(JNIEnv *env, jobject thiz)
{
    jboolean ret_value = J4AC_com_qmedia_qmediasdk_QCommon_QRange__isValid(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return false;
    }

    return ret_value;
}

jlong J4AC_com_qmedia_qmediasdk_QCommon_QRange__getLength(JNIEnv *env, jobject thiz)
{
    return (*env)->CallLongMethod(env, thiz, class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.method_getLength);
}

jlong J4AC_com_qmedia_qmediasdk_QCommon_QRange__getLength__catchAll(JNIEnv *env, jobject thiz)
{
    jlong ret_value = J4AC_com_qmedia_qmediasdk_QCommon_QRange__getLength(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

jlong J4AC_com_qmedia_qmediasdk_QCommon_QRange__start__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetLongField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.field_start);
}

jlong J4AC_com_qmedia_qmediasdk_QCommon_QRange__start__get__catchAll(JNIEnv *env, jobject thiz)
{
    jlong ret_value = J4AC_com_qmedia_qmediasdk_QCommon_QRange__start__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QCommon_QRange__start__set(JNIEnv *env, jobject thiz, jlong value)
{
    (*env)->SetLongField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.field_start, value);
}

void J4AC_com_qmedia_qmediasdk_QCommon_QRange__start__set__catchAll(JNIEnv *env, jobject thiz, jlong value)
{
    J4AC_com_qmedia_qmediasdk_QCommon_QRange__start__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

jlong J4AC_com_qmedia_qmediasdk_QCommon_QRange__end__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetLongField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.field_end);
}

jlong J4AC_com_qmedia_qmediasdk_QCommon_QRange__end__get__catchAll(JNIEnv *env, jobject thiz)
{
    jlong ret_value = J4AC_com_qmedia_qmediasdk_QCommon_QRange__end__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env)) {
        return 0;
    }

    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QCommon_QRange__end__set(JNIEnv *env, jobject thiz, jlong value)
{
    (*env)->SetLongField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.field_end, value);
}

void J4AC_com_qmedia_qmediasdk_QCommon_QRange__end__set__catchAll(JNIEnv *env, jobject thiz, jlong value)
{
    J4AC_com_qmedia_qmediasdk_QCommon_QRange__end__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

int J4A_loadClass__J4AC_com_qmedia_qmediasdk_QCommon_QRange(JNIEnv *env)
{
    int         ret                   = -1;
    const char *J4A_UNUSED(name)      = NULL;
    const char *J4A_UNUSED(sign)      = NULL;
    jclass      J4A_UNUSED(class_id)  = NULL;
    int         J4A_UNUSED(api_level) = 0;

    if (class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.id != NULL)
        return 0;

    sign = "com/qmedia/qmediasdk/QCommon/QRange";
    class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.id = J4A_FindClass__asGlobalRef__catchAll(env, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.id == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.id;
    name     = "<init>";
    sign     = "(JJ)V";
    class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.constructor_QRange = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.constructor_QRange == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.id;
    name     = "isValid";
    sign     = "()Z";
    class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.method_isValid = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.method_isValid == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.id;
    name     = "getLength";
    sign     = "()J";
    class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.method_getLength = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.method_getLength == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.id;
    name     = "start";
    sign     = "J";
    class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.field_start = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.field_start == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.id;
    name     = "end";
    sign     = "J";
    class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.field_end = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QCommon_QRange.field_end == NULL)
        goto fail;

    J4A_ALOGD("J4ALoader: OK: '%s' loaded\n", "com.qmedia.qmediasdk.QCommon.QRange");
    ret = 0;
fail:
    return ret;
}
