//
//  MediaEngineImpl.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef QMEDIA_API_MEDIAENGINE_IMPL_H
#define QMEDIA_API_MEDIAENGINE_IMPL_H

#include "media_engine.h"
#include "audio_description.h"

namespace QMedia { namespace Api {

class MediaEngineImpl {
public:
    static MediaEngineImpl* instance();
    /** set audio output paramenters */
    void config_audio_out(const AudioDescription & desc) {
        audio_out_ = desc;
    }
    
    const AudioDescription get_audio_out() const {
        return audio_out_;
    }
protected:
    MediaEngineImpl() = default;
    ~MediaEngineImpl() = default;
private:
    AudioDescription audio_out_{AudioDescription::FORMAT_S16, 2, 44100, 16};
};

}
}

#endif //QMEDIA_API_MEDIAENGINE_IMPL_H
