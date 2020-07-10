//
//  OutputTarget.h
//  QMediaCore
//
//  Created by spring on 2019/2/18.
//  Copyright © 2019 QMedia. All rights reserved.
//

#ifndef MEDIACORE_OUTPUTTARGET_H
#define MEDIACORE_OUTPUTTARGET_H

#include "MediaCore/core/MediaComm.h"

class OutputTarget {
public:
    OutputTarget() {}
    virtual ~OutputTarget() {}
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual void pause(bool isPause) = 0;
    virtual void flush() = 0;
protected:
    OutputTarget(const OutputTarget&) = delete;
    OutputTarget& operator=(const OutputTarget&) = delete;
};

#endif /* MEDIACORE_OUTPUTTARGET_H */
