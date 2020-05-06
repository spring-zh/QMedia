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
	DecodedFrameQueue(){}
	virtual ~DecodedFrameQueue(){}

	inline Range<int64_t> getCacheTimeRang() const { return _cacheTimeRang; }

	inline size_t getQueueConut() const { return _frame_list.size(); }

	void pushFrame(FrameT& frame, bool byOrder = false)
	{
		std::unique_lock<std::mutex> lck(_mutex);
		if (byOrder)
		{
			for (auto iter = _frame_list.begin(); iter!= _frame_list.end() ;++iter) {
				if (iter->render_time_ms() > frame.render_time_ms())
				{
					if(iter == _frame_list.begin())
						_cacheTimeRang._start = _frame_list.front().render_time_ms();

					_frame_list.insert(iter, frame);
					return;
				}
			}
		}
		
		_frame_list.push_back(frame);

		_cacheTimeRang._start = _frame_list.front().render_time_ms();
		_cacheTimeRang._end = _frame_list.back().render_time_ms();
	}

	FrameT popFrame() {
		FrameT retFrame(nullptr, 0);
		std::unique_lock<std::mutex> lck(_mutex);
		if (_frame_list.size() > 0)
		{
			retFrame = _frame_list.front();
			_frame_list.pop_front();
			if (_frame_list.size() > 0) {
				_cacheTimeRang._start = _frame_list.front().render_time_ms();
				_cacheTimeRang._end = _frame_list.back().render_time_ms();
			}
			else
			{
				_cacheTimeRang._start = -1;
				_cacheTimeRang._end = -1;
			}
		}
		return std::move(retFrame);
	}

	inline void clear() {
		std::unique_lock<std::mutex> lck(_mutex);
		_frame_list.clear();
		_cacheTimeRang._start = -1;
		_cacheTimeRang._end = -1;
	}

private:
	Range<int64_t> _cacheTimeRang;

	std::mutex _mutex;
	std::list<FrameT> _frame_list;
};
