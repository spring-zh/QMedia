//
//  CodecSpecificInfo.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include <vector>
#include <string.h>

#include "EncodedPacket.h"

class CodecSpecificInfo
{
public:

    CodecSpecificInfo() = default;

    ~CodecSpecificInfo() = default;
    
    CodecSpecificInfo(const CodecSpecificInfo& from): extra_datas_(from.extra_datas_) {}
    
	CodecSpecificInfo(CodecSpecificInfo&& from) : extra_datas_(std::move(from.extra_datas_)) {}
    
    CodecSpecificInfo& operator=(const CodecSpecificInfo& rhs) {
        extra_datas_ = rhs.extra_datas_;
        return *this;
    }
    
    CodecSpecificInfo& operator=(const CodecSpecificInfo&& rhs) {
        extra_datas_ = std::move(rhs.extra_datas_);
        return *this;
    }
    
    bool IsSameCodec(const CodecSpecificInfo& other) const {
        if (extra_datas_.size() == other.extra_datas_.size()) {
            for (int i = 0; i < extra_datas_.size(); ++i) {
                if (extra_datas_[i].size() != other.extra_datas_[i].size()) {
                    return false;
                }
                if (memcmp(extra_datas_[i].data(), other.extra_datas_[i].data(), extra_datas_[i].size())) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

	void addExtraData(uint8_t *data, size_t size) {
		std::vector<uint8_t > add_data(data, data + size);
		extra_datas_.push_back(add_data);
	}

	const std::vector<std::vector<uint8_t >>& getExtraDatas() const {
		return extra_datas_;
	}
    
    int GetCodecFlag() const { return codec_flag_; }
    void SetCodecFlag(int codec_flag) { codec_flag_ = codec_flag; }

private:
    int codec_flag_ = 0;
	std::vector<std::vector<uint8_t >> extra_datas_;
};

class CodecSpecificData : public EncodedPacketBuffer{
public:
	CodecSpecificData(CodecSpecificInfo& specificInfo) : 
		_specificInfo(specificInfo){

	}
	virtual int64_t pts() const { return 0; }
	virtual int64_t dts() const { return 0; }
	virtual const uint8_t *data() const { return (const uint8_t *)&_specificInfo; }
	virtual size_t size() const { return 0; }

	virtual bool bframe() const { return false; }

	virtual bool isCompleteFrame() const { return false; }

	CodecSpecificInfo& getCodecSpecificInfo() { return _specificInfo; }

private:

	CodecSpecificInfo _specificInfo;
};
