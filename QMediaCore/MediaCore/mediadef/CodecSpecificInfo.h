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

	typedef struct ExtraItem
	{
		uint8_t* data;
		size_t datalen;
	}ExtraItem;

	CodecSpecificInfo():handle_(NULL){}

	~CodecSpecificInfo() {
		for (auto iter : extra_datas_)
		{
			if (iter.data != NULL)
			{
				delete iter.data;
			}
		}
		extra_datas_.clear();
	}
	CodecSpecificInfo(const CodecSpecificInfo& from) {
		auto datas = from.getExtraData();
		for (auto iter : datas)
		{
			ExtraItem add_item;
			add_item.datalen = iter.datalen;
			add_item.data = new uint8_t[iter.datalen];
			memcpy(add_item.data, iter.data, iter.datalen);
			extra_datas_.push_back(add_item);
		}
	}
	CodecSpecificInfo(CodecSpecificInfo&& from) {
		auto datas = from.getExtraData();
		for (auto iter : datas)
		{
			extra_datas_.push_back(iter);
		}
		datas.clear();
	}

	void addExtraDataFrom(uint8_t *data, size_t size) {
		if (data)
		{
			ExtraItem add_item;
			add_item.datalen = size;
			add_item.data = new uint8_t[size];
			memcpy(add_item.data, data, size);
			extra_datas_.push_back(add_item);
		}
	}

	void setOutPutHandle(void *handle) { handle_ = handle; }
	const void * getOutPutHandle() const { return handle_; }

	const std::vector<ExtraItem>& getExtraData() const {
		return extra_datas_;
	}

private:

	std::vector<ExtraItem> extra_datas_;

	void *handle_;
};

class CodecSpecificData : public EncodedData{
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
