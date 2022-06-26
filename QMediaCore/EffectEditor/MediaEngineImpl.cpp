//
//  MediaEngine.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaEngineImpl.h"
#include "MediaSegmentImpl.h"
#include "EditorPlayerImpl.h"
#include "EditorExporterImpl.h"

namespace QMedia { namespace Api {

MediaEngineImpl* MediaEngineImpl::instance() {
    static MediaEngineImpl impl;
    return &impl;
}

std::string MediaEngine::version() {
    return "1.0.1";
}

/** set audio output paramenters */
void MediaEngine::config_audio_out(const AudioDescription & desc) {
    MediaEngineImpl::instance()->config_audio_out(desc);
}

AudioDescription MediaEngine::audio_out() {
    return MediaEngineImpl::instance()->get_audio_out();
}

std::shared_ptr<EditorPlayerInternal> MediaEngine::create_editor_player_internal() {
    return std::shared_ptr<EditorPlayerInternal>(new EditorPlayerImpl());
}

std::shared_ptr<EditorExporterInternal> MediaEngine::create_editor_exporter_internal() {
    return std::shared_ptr<EditorExporterInternal>(new EditorExporterImpl());
}

}
}
