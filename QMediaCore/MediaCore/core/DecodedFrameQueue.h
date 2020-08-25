//
//  DecodedFrameQueue.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "Utils/Comm.h"
#include <list>
#include <mutex>

template <class FrameT>
class DecodedFrameQueue
{
public:
	DecodedFrameQueue():_isAbort(false) {}
	virtual ~DecodedFrameQueue(){}

	inline Range<int64_t> getCacheTimeRang() const { return _cacheTimeRang; }

	inline size_t getQueueConut() const { return _frame_list.size(); }

	void setAbort(bool abort) {
		std::unique_lock<std::mutex> lck(_mutex);
		_isAbort = abort;
		_condition.notify_one();
	}


	void pushFrame(FrameT& frame, bool byOrder = true)
	{
		std::unique_lock<std::mutex> lck(_mutex);
		if (byOrder)
		{
			auto iter = _frame_list.begin();
			for (; iter!= _frame_list.end() ;++iter) {
				if (iter->render_time_ms() > frame.render_time_ms())
					break;
			}
			_frame_list.insert(iter, frame);
		} else {
			_frame_list.push_back(frame);
		}
		_condition.notify_one();
		_cacheTimeRang._start = _frame_list.front().render_time_ms();
		_cacheTimeRang._end = _frame_list.back().render_time_ms();
	}

	FrameT popFrame() {
		FrameT retFrame(nullptr, 0);

		std::unique_lock<std::mutex> lock(_mutex);
		while (_frame_list.empty() && !_isAbort) {
			_condition.wait(lock, [this]{return !_frame_list.empty() || _isAbort;});
		}

		if (! _frame_list.empty())
		{
			retFrame = _frame_list.front();
			_frame_list.pop_front();
			if (! _frame_list.empty()) {
				_cacheTimeRang._start = _frame_list.front().render_time_ms();
			}
		}
		return retFrame;
	}

	FrameT peekFrame() {
		FrameT retFrame(nullptr, 0);

		std::unique_lock<std::mutex> lock(_mutex);
		while (_frame_list.empty() && !_isAbort) {
			_condition.wait(lock, [this]{return !_frame_list.empty() || _isAbort;});
		}

		if (! _frame_list.empty())
		{
			retFrame = _frame_list.front();
		}
		return retFrame;
	}

	inline size_t length() const { return _frame_list.size(); }

	inline void clearAll() {
		std::unique_lock<std::mutex> lck(_mutex);
		_frame_list.clear();
		_cacheTimeRang = Range<int64_t>();
	}

private:
	Range<int64_t> _cacheTimeRang;
	bool _isAbort;
	std::condition_variable _condition;
	std::mutex _mutex;
	std::list<FrameT> _frame_list;
};
