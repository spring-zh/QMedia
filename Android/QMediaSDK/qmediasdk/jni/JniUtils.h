//
// Created by spring on 2017/5/1.
//

#ifndef PSSDKTEST_JNICLASS_H
#define PSSDKTEST_JNICLASS_H

#include <jni.h>
#include <pthread.h>
#include <functional>
#include <mutex>

namespace JniUtils {
/*
 * Jni field type.
 */
enum JniFieldType {

    FF_JNI_CLASS,
    FF_JNI_FIELD,
    FF_JNI_STATIC_FIELD,
    FF_JNI_METHOD,
    FF_JNI_STATIC_METHOD

};
/*
 * Jni field describing a class, a field or a method to be retrieved using
 * the ff_jni_init_jfields method.
 */
struct JniField {

    const char *name;
    const char *method;
    const char *signature;
    enum JniFieldType type;
    int offset;
    int mandatory;

};


void setVM(JavaVM *vm);
JavaVM *getVM();

JNIEnv *getEnv();
JNIEnv *getEnvDirect();
char *jni_jstring_to_utf_chars(JNIEnv *env, jstring string);
jstring utf_chars_to_jstring(JNIEnv *env, const char *utf_chars);
void jni_detach_env(void *data);

int jni_init_jfields(JNIEnv *env, void *jfields, const struct JniField *jfields_mapping, int global);
int jni_reset_jfields(JNIEnv *env, void *jfields, const struct JniField *jfields_mapping, int global);
int jni_exception_get_summary(JNIEnv *env, jthrowable exception, char **error);
int jni_exception_check(JNIEnv *env);

jlong getObjectPtr(JNIEnv* env, jobject thiz);
bool setObjectPtr(JNIEnv* env, jobject thiz, jlong ptr);


class JGlobalObject {
public:
    JGlobalObject(jobject obj) {
        JNIEnv* env = getEnv();
        _obj = env->NewGlobalRef(obj);
    }
    ~JGlobalObject() {
        JNIEnv* env = getEnv();
        env->DeleteGlobalRef(_obj);
    }

    jobject operator()(){
        return _obj;
    }

protected:
    jobject _obj;
};

class JWeakObject {
public:
    JWeakObject(jobject obj) {
        JNIEnv* env = getEnv();
        _obj = env->NewWeakGlobalRef(obj);
    }
    ~JWeakObject() {
        JNIEnv* env = getEnv();
        env->DeleteWeakGlobalRef(_obj);
    }

    jobject operator()(){
        return _obj;
    }

protected:
    jobject _obj;
};

}

#endif //PSSDKTEST_JNICLASS_H
