//
//  EncodedPacket.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "MediaComm.h"
#include <stdint.h>
#include <memory>

constexpr unsigned BitValue(unsigned n) {
	return 1 << n;
}
inline bool HasValue(unsigned a, unsigned val) { return a & val; }

class EncodedData {
public:
	virtual ~EncodedData() {}

	virtual int64_t pts() const = 0;
	virtual int64_t dts() const = 0;
	virtual const uint8_t *data() const = 0;
	virtual size_t size() const = 0;

	virtual bool bframe() const = 0;

	virtual bool isCompleteFrame() const = 0;
};

class GeneralEncodedData : public EncodedData
{
public:
	GeneralEncodedData():
		_data(NULL),
		_size(0){
		set_value(0, 0, false, false);
	}
	~GeneralEncodedData() {
		releaseBuffer();
	}

	GeneralEncodedData(const GeneralEncodedData& from) {
		copyFrom(from._data, from.size);
		set_value(from._pts, from._dts, from._is_bframe, from._isCompleteFrame);
	}

	GeneralEncodedData(GeneralEncodedData&& from) {
		_data = from._data;
		_size = from._size;
		set_value(from._pts, from._dts, from._is_bframe, from._isCompleteFrame);
		from.set_value(0, 0, false, false);
		from._data = NULL;
		from._size = 0;
	}

	void copyFrom(const uint8_t* src, size_t src_size) {
		if (!(src && src_size > 0))
			return;

		if (_data && _size > src_size) {
			memcpy(_data, src, src_size);
			_size = src_size;
		}
		else
		{
			_data = (uint8_t*)realloc(_data, src_size);
			memcpy(_data, src, src_size);
			_size = src_size;
		}
	}

	int64_t pts() const { return _pts; }
	int64_t dts() const { return _dts; }
	const uint8_t *data() { return _data; }
	size_t size() { return _size; }

	bool bframe() const { return _is_bframe; }

	bool isCompleteFrame() const { return _isCompleteFrame; }

	int64_t _pts;
	int64_t _dts;
	uint8_t *_data;
	size_t _size;

	bool _is_bframe;
	bool _isCompleteFrame;

private:
	void set_value(int64_t pts, int64_t dts, bool bframe, bool isCompleteFrame) {
		_pts = pts; _dts = dts, _is_bframe = bframe, _isCompleteFrame = isCompleteFrame;
	}
	void releaseBuffer() {
		if (_data) {
			delete _data;
			_data = NULL;
		}
		_size = 0;
	}

};

class EncodedPacket
{
public:

	enum :unsigned{ FLAG_SYNC = BitValue(0), FLAG_FLUSH = BitValue(1), FLAG_CONFIG = BitValue(2), FLAG_END = BitValue(3)};

	EncodedPacket(std::shared_ptr<EncodedData> _encData, int64_t pts_ms):
		_pts_ms(pts_ms),
		_flags(0) {}
	virtual ~EncodedPacket(){}

	// Support move and copy.
	EncodedPacket(const EncodedPacket&) = default;
	EncodedPacket(EncodedPacket&&) = default;
	EncodedPacket& operator=(const EncodedPacket&) = default;
	EncodedPacket& operator=(EncodedPacket&&) = default;

	virtual int64_t pts() const { return _encData->pts(); }
	virtual int64_t dts() const{ return _encData->dts(); }
	virtual int64_t pts_ms() const { return _pts_ms; }
	virtual void set_pts_ms(int64_t pts_ms) { _pts_ms = pts_ms; }

	inline uint32_t flags() const { return _flags; }
	inline void setFlags(uint32_t flags) { _flags = flags;}

	virtual const std::shared_ptr<EncodedData>& getEncodedData() { return _encData; }

protected:

	int64_t _pts_ms;
	int64_t _ntp_time_ms = 0;
	int64_t _capture_time_ms = 0;

	uint32_t _flags;

	std::shared_ptr<EncodedData> _encData;
	MediaDescribe _mediaDesc;
};

