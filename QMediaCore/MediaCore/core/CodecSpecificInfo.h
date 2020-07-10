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

	CodecSpecificInfo() {}

	~CodecSpecificInfo() {
		extra_datas_.clear();
	}
	CodecSpecificInfo(const CodecSpecificInfo& from) {
		extra_datas_ = from.getExtraDatas();
	}
	CodecSpecificInfo(CodecSpecificInfo&& from) {
		extra_datas_ = std::move(from.getExtraDatas());
	}

	void addExtraData(uint8_t *data, size_t size) {
		std::vector<uint8_t > add_data(data, data + size);
		extra_datas_.push_back(add_data);
	}

	const std::vector<std::vector<uint8_t >>& getExtraDatas() const {
		return extra_datas_;
	}

private:
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
