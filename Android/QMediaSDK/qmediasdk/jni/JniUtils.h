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

#define OBJECT_PTR_FIELD "mPtr"
jlong getObjectPtr(JNIEnv* env, jobject thiz);
jlong getObjectPtr(JNIEnv* env, jobject thiz, const char* field);
bool setObjectPtr(JNIEnv* env, jobject thiz, jlong ptr, const char* field);
bool setObjectPtr(JNIEnv* env, jobject thiz, jlong ptr);

class JGlobalObject {
public:
    JGlobalObject(jobject object) {
        JNIEnv* env = getEnv();
        _object = env->NewGlobalRef(object);
    }
    ~JGlobalObject() {
        JNIEnv* env = getEnv();
        env->DeleteGlobalRef(_object);
    }

    operator jobject() const{
        return _object;
    }

protected:
    jobject _object;
};

class JWeakObject {
public:
    JWeakObject(jobject object) {
        JNIEnv* env = getEnv();
        _object = env->NewWeakGlobalRef(object);
    }
    ~JWeakObject() {
        JNIEnv* env = getEnv();
        env->DeleteWeakGlobalRef(_object);
    }

    inline operator jobject() const {
        return _object;
    }

protected:
    jobject _object;
};

}

#endif //PSSDKTEST_JNICLASS_H
