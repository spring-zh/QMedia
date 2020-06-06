//
//  QPlayer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "QPlayer.h"

enum AsyncCommand : int{
    CMD_UNKNOW = -1,
    CMD_PREPARE = 1,
    CMD_OPEN,
    CMD_CLOSE,
    CMD_PAUSE,
    CMD_SEEK,
    CMD_PLAY,
    CMD_COMPLETED
};

QPlayer::QPlayer(VideoDecoderFactory* vdFactory, AudioDecoderFactory* adFactory) {
    
}
QPlayer::~QPlayer() {
    
}

void QPlayer::setVideoTarget(VideoTarget* videoTarget)
{
    _videoTarget = videoTarget;
    _videoTarget->setRender(this);
    if (_videoTarget && _audioTarget) {
        _state = Initialized;
    }
}
void QPlayer::setAudioTarget(AudioTarget* audioTarget)
{
    _audioTarget = audioTarget;
    _audioTarget->setRender(this);
    if (_videoTarget && _audioTarget) {
        _state = Initialized;
    }
}

QPlayer::RetCode QPlayer::open(std::string url) {
    if (_threadTask.isCurrent()) {
        //TODO: handle open
        return QPlayer::unknow;
    }else {
        _threadTask.postTask((int)CMD_OPEN, &QPlayer::open, this, url);
        return QPlayer::ok;
    }
}
QPlayer::RetCode QPlayer::play() {
    if (_threadTask.isCurrent()) {
        //TODO: handle play
        return QPlayer::unknow;
    }else {
        _threadTask.postTask((int)CMD_PLAY, &QPlayer::play, this);
        return QPlayer::ok;
    }
}
QPlayer::RetCode QPlayer::pause() {
    if (_threadTask.isCurrent()) {
        //TODO: handle pause
        return QPlayer::unknow;
    }else {
        _threadTask.postTask((int)CMD_PAUSE, &QPlayer::pause, this);
        return QPlayer::ok;
    }
}
QPlayer::RetCode QPlayer::seek(int64_t mSec, int flag) {
    if (_threadTask.isCurrent()) {
        //TODO: handle seek
        return QPlayer::unknow;
    }else {
        _threadTask.removeTaskById(CMD_SEEK);
        _threadTask.postTask((int)CMD_SEEK, &QPlayer::seek, this, mSec, flag);
        return QPlayer::ok;
    }
}
QPlayer::RetCode QPlayer::close() {
    if (_threadTask.isCurrent()) {
        //TODO: handle close
        return QPlayer::unknow;
    }else {
        _threadTask.postTask((int)CMD_CLOSE, &QPlayer::close, this);
        return QPlayer::ok;
    }
}

bool QPlayer::onRenderCreate() {
    return true;
}
bool QPlayer::onVideoRender(int64_t wantTimeMs) {
    return true;
}
bool QPlayer::onRenderDestroy() {
    return true;
}
bool QPlayer::onAudioRender(uint8_t * const buffer, unsigned needBytes, int64_t wantTimeMs) {
    return true;
}
