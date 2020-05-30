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

#ifndef J4A__com_qmedia_qmediasdk_QTarget_QVideoTarget__H
#define J4A__com_qmedia_qmediasdk_QTarget_QVideoTarget__H

#include "j4a/j4a_base.h"

jboolean J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__startVideo(JNIEnv *env, jobject thiz);
jboolean J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__startVideo__catchAll(JNIEnv *env, jobject thiz);
jboolean J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__stopVideo(JNIEnv *env, jobject thiz);
jboolean J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__stopVideo__catchAll(JNIEnv *env, jobject thiz);
void J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__pauseVideo(JNIEnv *env, jobject thiz, jboolean isPause);
void J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__pauseVideo__catchAll(JNIEnv *env, jobject thiz, jboolean isPause);
void J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__flushVideo(JNIEnv *env, jobject thiz);
void J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__flushVideo__catchAll(JNIEnv *env, jobject thiz);
jboolean J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__initVideo(JNIEnv *env, jobject thiz, jobject describe);
jboolean J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__initVideo__catchAll(JNIEnv *env, jobject thiz, jobject describe);
jint J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetWidth(JNIEnv *env, jobject thiz);
jint J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetWidth__catchAll(JNIEnv *env, jobject thiz);
jint J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetHeight(JNIEnv *env, jobject thiz);
jint J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetHeight__catchAll(JNIEnv *env, jobject thiz);
jfloat J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getFrameRate(JNIEnv *env, jobject thiz);
jfloat J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getFrameRate__catchAll(JNIEnv *env, jobject thiz);
jobject J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetContext(JNIEnv *env, jobject thiz);
jobject J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetContext__catchAll(JNIEnv *env, jobject thiz);
jobject J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetContext__asGlobalRef__catchAll(JNIEnv *env, jobject thiz);
void J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__setVideoRender(JNIEnv *env, jobject thiz, jobject videoRender);
void J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__setVideoRender__catchAll(JNIEnv *env, jobject thiz, jobject videoRender);
jobject J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getVideoRender(JNIEnv *env, jobject thiz);
jobject J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getVideoRender__catchAll(JNIEnv *env, jobject thiz);
jobject J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getVideoRender__asGlobalRef__catchAll(JNIEnv *env, jobject thiz);
int J4A_loadClass__J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget(JNIEnv *env);

#endif//J4A__com_qmedia_qmediasdk_QTarget_QVideoTarget__H
