//
//  EncodedPacket.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include "Utils/Comm.h"
#include "MediaComm.h"

class EncodedPacketBuffer {
public:
	virtual ~EncodedPacketBuffer() {}

	virtual int64_t pts() const = 0;
	virtual int64_t dts() const = 0;
	virtual const uint8_t *data() const = 0;
	virtual size_t size() const = 0;

	virtual bool is_keyframe() const = 0;
	virtual bool bframe() const = 0;

	virtual bool isCompleteFrame() const = 0;

	virtual int stream_id() const = 0;
};

class GeneralEncodedData : public EncodedPacketBuffer
{
public:
	GeneralEncodedData(){
		set_value(0, 0, false, false, -1);
	}
	~GeneralEncodedData() = default;

	GeneralEncodedData(const GeneralEncodedData& from) {
		_data = from._data;
		set_value(from._pts, from._dts, from._is_bframe, from._isCompleteFrame, from._stream_id);
	}

	GeneralEncodedData(GeneralEncodedData&& from) {
		_data = std::move(from._data);
		set_value(from._pts, from._dts, from._is_bframe, from._isCompleteFrame, from._stream_id);
	}

	void copyFrom(const uint8_t* src, size_t src_size) {
		if (!(src && src_size > 0))
			return;

		_data.assign(src, src + src_size);
	}

	int64_t pts() const override { return _pts; }
	int64_t dts() const override { return _dts; }
	const uint8_t *data() const override { return _data.data(); }
	size_t size() const override { return _data.size(); }

	int stream_id() const override { return _stream_id; }

	bool bframe() const override { return _is_bframe; }

	bool isCompleteFrame() const override { return _isCompleteFrame; }

protected:
	int64_t _pts;
	int64_t _dts;
	std::vector<uint8_t > _data;

	bool _is_bframe;
	bool _isCompleteFrame;

	int _stream_id;

private:
	void set_value(int64_t pts, int64_t dts, bool bframe, bool isCompleteFrame, int stream_id) {
		_stream_id = stream_id;
		_pts = pts; _dts = dts, _is_bframe = bframe, _isCompleteFrame = isCompleteFrame;
	}
};

class EncodedPacket
{
public:

	enum :unsigned{ FLAG_SYNC = BitValue(0), FLAG_FLUSH = BitValue(1), FLAG_CONFIG = BitValue(2), FLAG_END = BitValue(3), FLAG_DISCARD = BitValue(4)};

	EncodedPacket(std::shared_ptr<EncodedPacketBuffer> _encData, int64_t ntp_time_ms):
            _encodedBuffer(_encData),
            _ntp_time_ms(ntp_time_ms),
            _flags(0) {
		    }
	virtual ~EncodedPacket(){}

	// Support move and copy.
	EncodedPacket(const EncodedPacket&) = default;
	EncodedPacket(EncodedPacket&&) = default;
	EncodedPacket& operator=(const EncodedPacket&) = default;
	EncodedPacket& operator=(EncodedPacket&&) = default;

	virtual int64_t pts() const { return _encodedBuffer->pts(); }
	virtual int64_t dts() const{ return _encodedBuffer->dts(); }
	virtual int64_t ntp_time_ms() const { return _ntp_time_ms; }
	virtual void set_ntp_time_ms(int64_t time_ms) { _ntp_time_ms = time_ms; }

	inline uint32_t flags() const { return _flags; }
	inline void setFlags(uint32_t flags) { _flags = flags;}

	virtual const std::shared_ptr<EncodedPacketBuffer>& getEncodedBuffer() const { return _encodedBuffer; }

protected:

	int64_t _ntp_time_ms = 0;
	int64_t _capture_time_ms = 0;

	uint32_t _flags;

	std::shared_ptr<EncodedPacketBuffer> _encodedBuffer;
};

