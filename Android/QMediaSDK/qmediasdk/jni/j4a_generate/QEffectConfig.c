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

#include "QEffectConfig.h"

typedef struct J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig {
    jclass id;

    jmethodID constructor_QEffectConfig;
    jfieldID field_name;
    jfieldID field_describe;
    jfieldID field_type;
} J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig;
static J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig;

jobject J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig(JNIEnv *env, jstring name, jstring describe, jstring type)
{
    return (*env)->NewObject(env, class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.id, class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.constructor_QEffectConfig, name, describe, type);
}

jobject J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig__catchAll(JNIEnv *env, jstring name, jstring describe, jstring type)
{
    jobject ret_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig(env, name, describe, type);
    if (J4A_ExceptionCheck__catchAll(env) || !ret_object) {
        return NULL;
    }

    return ret_object;
}

jobject J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig__asGlobalRef__catchAll(JNIEnv *env, jstring name, jstring describe, jstring type)
{
    jobject ret_object   = NULL;
    jobject local_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig__catchAll(env, name, describe, type);
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

jobject J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig__withCString(JNIEnv *env, const char *name_cstr__, const char *describe_cstr__, const char *type_cstr__)
{
    jobject ret_object = NULL;
    jstring name = NULL;
    jstring describe = NULL;
    jstring type = NULL;

    name = (*env)->NewStringUTF(env, name_cstr__);
    if (J4A_ExceptionCheck__throwAny(env) || !name)
        goto fail;
    describe = (*env)->NewStringUTF(env, describe_cstr__);
    if (J4A_ExceptionCheck__throwAny(env) || !describe)
        goto fail;
    type = (*env)->NewStringUTF(env, type_cstr__);
    if (J4A_ExceptionCheck__throwAny(env) || !type)
        goto fail;

    ret_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig(env, name, describe, type);
    if (J4A_ExceptionCheck__throwAny(env) || !ret_object) {
        ret_object = NULL;
        goto fail;
    }

fail:
    J4A_DeleteLocalRef__p(env, &name);
    J4A_DeleteLocalRef__p(env, &describe);
    J4A_DeleteLocalRef__p(env, &type);
    return ret_object;
}

jobject J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig__withCString__catchAll(JNIEnv *env, const char *name_cstr__, const char *describe_cstr__, const char *type_cstr__)
{
    jobject ret_object = NULL;
    jstring name = NULL;
    jstring describe = NULL;
    jstring type = NULL;

    name = (*env)->NewStringUTF(env, name_cstr__);
    if (J4A_ExceptionCheck__catchAll(env) || !name)
        goto fail;
    describe = (*env)->NewStringUTF(env, describe_cstr__);
    if (J4A_ExceptionCheck__catchAll(env) || !describe)
        goto fail;
    type = (*env)->NewStringUTF(env, type_cstr__);
    if (J4A_ExceptionCheck__catchAll(env) || !type)
        goto fail;

    ret_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig__catchAll(env, name, describe, type);
    if (J4A_ExceptionCheck__catchAll(env) || !ret_object) {
        ret_object = NULL;
        goto fail;
    }

fail:
    J4A_DeleteLocalRef__p(env, &name);
    J4A_DeleteLocalRef__p(env, &describe);
    J4A_DeleteLocalRef__p(env, &type);
    return ret_object;
}

jobject J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig__withCString__asGlobalRef__catchAll(JNIEnv *env, const char *name_cstr__, const char *describe_cstr__, const char *type_cstr__)
{
    jobject ret_object   = NULL;
    jobject local_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__QEffectConfig__withCString__catchAll(env, name_cstr__, describe_cstr__, type_cstr__);
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

jstring J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetObjectField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_name);
}

jstring J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__get__catchAll(JNIEnv *env, jobject thiz)
{
    jstring ret_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env) || !ret_object) {
        return NULL;
    }

    return ret_object;
}

jstring J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__get__asGlobalRef__catchAll(JNIEnv *env, jobject thiz)
{
    jstring ret_object   = NULL;
    jstring local_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__get__catchAll(env, thiz);
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

const char *J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__get__asCBuffer(JNIEnv *env, jobject thiz, char *out_buf, int out_len)
{
    const char *ret_value = NULL;
    const char *c_str     = NULL;
    jstring local_string = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__get(env, thiz);
    if (J4A_ExceptionCheck__throwAny(env) || !local_string) {
        goto fail;
    }

    c_str = (*env)->GetStringUTFChars(env, local_string, NULL );
    if (J4A_ExceptionCheck__throwAny(env) || !c_str) {
        goto fail;
    }

    strlcpy(out_buf, c_str, out_len);
    ret_value = out_buf;

fail:
    J4A_ReleaseStringUTFChars__p(env, local_string, &c_str);
    J4A_DeleteLocalRef__p(env, &local_string);
    return ret_value;
}

const char *J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__get__asCBuffer__catchAll(JNIEnv *env, jobject thiz, char *out_buf, int out_len)
{
    const char *ret_value = NULL;
    const char *c_str     = NULL;
    jstring local_string = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__get__catchAll(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env) || !local_string) {
        goto fail;
    }

    c_str = (*env)->GetStringUTFChars(env, local_string, NULL );
    if (J4A_ExceptionCheck__catchAll(env) || !c_str) {
        goto fail;
    }

    strlcpy(out_buf, c_str, out_len);
    ret_value = out_buf;

fail:
    J4A_ReleaseStringUTFChars__p(env, local_string, &c_str);
    J4A_DeleteLocalRef__p(env, &local_string);
    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__set(JNIEnv *env, jobject thiz, jstring value)
{
    (*env)->SetObjectField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_name, value);
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__set__catchAll(JNIEnv *env, jobject thiz, jstring value)
{
    J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__set__withCString(JNIEnv *env, jobject thiz, const char *value_cstr__)
{
    jstring value = NULL;

    value = (*env)->NewStringUTF(env, value_cstr__);
    if (J4A_ExceptionCheck__throwAny(env) || !value)
        goto fail;

    J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__set(env, thiz, value);

fail:
    J4A_DeleteLocalRef__p(env, &value);
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__set__withCString__catchAll(JNIEnv *env, jobject thiz, const char *value_cstr__)
{
    jstring value = NULL;

    value = (*env)->NewStringUTF(env, value_cstr__);
    if (J4A_ExceptionCheck__catchAll(env) || !value)
        goto fail;

    J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__name__set__catchAll(env, thiz, value);

fail:
    J4A_DeleteLocalRef__p(env, &value);
}

jstring J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetObjectField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_describe);
}

jstring J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__get__catchAll(JNIEnv *env, jobject thiz)
{
    jstring ret_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env) || !ret_object) {
        return NULL;
    }

    return ret_object;
}

jstring J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__get__asGlobalRef__catchAll(JNIEnv *env, jobject thiz)
{
    jstring ret_object   = NULL;
    jstring local_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__get__catchAll(env, thiz);
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

const char *J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__get__asCBuffer(JNIEnv *env, jobject thiz, char *out_buf, int out_len)
{
    const char *ret_value = NULL;
    const char *c_str     = NULL;
    jstring local_string = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__get(env, thiz);
    if (J4A_ExceptionCheck__throwAny(env) || !local_string) {
        goto fail;
    }

    c_str = (*env)->GetStringUTFChars(env, local_string, NULL );
    if (J4A_ExceptionCheck__throwAny(env) || !c_str) {
        goto fail;
    }

    strlcpy(out_buf, c_str, out_len);
    ret_value = out_buf;

fail:
    J4A_ReleaseStringUTFChars__p(env, local_string, &c_str);
    J4A_DeleteLocalRef__p(env, &local_string);
    return ret_value;
}

const char *J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__get__asCBuffer__catchAll(JNIEnv *env, jobject thiz, char *out_buf, int out_len)
{
    const char *ret_value = NULL;
    const char *c_str     = NULL;
    jstring local_string = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__get__catchAll(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env) || !local_string) {
        goto fail;
    }

    c_str = (*env)->GetStringUTFChars(env, local_string, NULL );
    if (J4A_ExceptionCheck__catchAll(env) || !c_str) {
        goto fail;
    }

    strlcpy(out_buf, c_str, out_len);
    ret_value = out_buf;

fail:
    J4A_ReleaseStringUTFChars__p(env, local_string, &c_str);
    J4A_DeleteLocalRef__p(env, &local_string);
    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__set(JNIEnv *env, jobject thiz, jstring value)
{
    (*env)->SetObjectField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_describe, value);
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__set__catchAll(JNIEnv *env, jobject thiz, jstring value)
{
    J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__set__withCString(JNIEnv *env, jobject thiz, const char *value_cstr__)
{
    jstring value = NULL;

    value = (*env)->NewStringUTF(env, value_cstr__);
    if (J4A_ExceptionCheck__throwAny(env) || !value)
        goto fail;

    J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__set(env, thiz, value);

fail:
    J4A_DeleteLocalRef__p(env, &value);
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__set__withCString__catchAll(JNIEnv *env, jobject thiz, const char *value_cstr__)
{
    jstring value = NULL;

    value = (*env)->NewStringUTF(env, value_cstr__);
    if (J4A_ExceptionCheck__catchAll(env) || !value)
        goto fail;

    J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__describe__set__catchAll(env, thiz, value);

fail:
    J4A_DeleteLocalRef__p(env, &value);
}

jstring J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__get(JNIEnv *env, jobject thiz)
{
    return (*env)->GetObjectField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_type);
}

jstring J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__get__catchAll(JNIEnv *env, jobject thiz)
{
    jstring ret_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__get(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env) || !ret_object) {
        return NULL;
    }

    return ret_object;
}

jstring J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__get__asGlobalRef__catchAll(JNIEnv *env, jobject thiz)
{
    jstring ret_object   = NULL;
    jstring local_object = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__get__catchAll(env, thiz);
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

const char *J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__get__asCBuffer(JNIEnv *env, jobject thiz, char *out_buf, int out_len)
{
    const char *ret_value = NULL;
    const char *c_str     = NULL;
    jstring local_string = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__get(env, thiz);
    if (J4A_ExceptionCheck__throwAny(env) || !local_string) {
        goto fail;
    }

    c_str = (*env)->GetStringUTFChars(env, local_string, NULL );
    if (J4A_ExceptionCheck__throwAny(env) || !c_str) {
        goto fail;
    }

    strlcpy(out_buf, c_str, out_len);
    ret_value = out_buf;

fail:
    J4A_ReleaseStringUTFChars__p(env, local_string, &c_str);
    J4A_DeleteLocalRef__p(env, &local_string);
    return ret_value;
}

const char *J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__get__asCBuffer__catchAll(JNIEnv *env, jobject thiz, char *out_buf, int out_len)
{
    const char *ret_value = NULL;
    const char *c_str     = NULL;
    jstring local_string = J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__get__catchAll(env, thiz);
    if (J4A_ExceptionCheck__catchAll(env) || !local_string) {
        goto fail;
    }

    c_str = (*env)->GetStringUTFChars(env, local_string, NULL );
    if (J4A_ExceptionCheck__catchAll(env) || !c_str) {
        goto fail;
    }

    strlcpy(out_buf, c_str, out_len);
    ret_value = out_buf;

fail:
    J4A_ReleaseStringUTFChars__p(env, local_string, &c_str);
    J4A_DeleteLocalRef__p(env, &local_string);
    return ret_value;
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__set(JNIEnv *env, jobject thiz, jstring value)
{
    (*env)->SetObjectField(env, thiz, class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_type, value);
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__set__catchAll(JNIEnv *env, jobject thiz, jstring value)
{
    J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__set(env, thiz, value);
    J4A_ExceptionCheck__catchAll(env);
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__set__withCString(JNIEnv *env, jobject thiz, const char *value_cstr__)
{
    jstring value = NULL;

    value = (*env)->NewStringUTF(env, value_cstr__);
    if (J4A_ExceptionCheck__throwAny(env) || !value)
        goto fail;

    J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__set(env, thiz, value);

fail:
    J4A_DeleteLocalRef__p(env, &value);
}

void J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__set__withCString__catchAll(JNIEnv *env, jobject thiz, const char *value_cstr__)
{
    jstring value = NULL;

    value = (*env)->NewStringUTF(env, value_cstr__);
    if (J4A_ExceptionCheck__catchAll(env) || !value)
        goto fail;

    J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig__type__set__catchAll(env, thiz, value);

fail:
    J4A_DeleteLocalRef__p(env, &value);
}

int J4A_loadClass__J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig(JNIEnv *env)
{
    int         ret                   = -1;
    const char *J4A_UNUSED(name)      = NULL;
    const char *J4A_UNUSED(sign)      = NULL;
    jclass      J4A_UNUSED(class_id)  = NULL;
    int         J4A_UNUSED(api_level) = 0;

    if (class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.id != NULL)
        return 0;

    sign = "com/qmedia/qmediasdk/QEffect/QEffectConfig";
    class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.id = J4A_FindClass__asGlobalRef__catchAll(env, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.id == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.id;
    name     = "<init>";
    sign     = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V";
    class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.constructor_QEffectConfig = J4A_GetMethodID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.constructor_QEffectConfig == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.id;
    name     = "name";
    sign     = "Ljava/lang/String;";
    class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_name = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_name == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.id;
    name     = "describe";
    sign     = "Ljava/lang/String;";
    class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_describe = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_describe == NULL)
        goto fail;

    class_id = class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.id;
    name     = "type";
    sign     = "Ljava/lang/String;";
    class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_type = J4A_GetFieldID__catchAll(env, class_id, name, sign);
    if (class_J4AC_com_qmedia_qmediasdk_QEffect_QEffectConfig.field_type == NULL)
        goto fail;

    J4A_ALOGD("J4ALoader: OK: '%s' loaded\n", "com.qmedia.qmediasdk.QEffect.QEffectConfig");
    ret = 0;
fail:
    return ret;
}
