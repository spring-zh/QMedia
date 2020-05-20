package com.qmedia.qmediasdk.QCommon;

import java.util.LinkedList;

//use to cache decode output buffer sort by pts
public class FrameCacheQueue<T extends HardwareDecoder.DecodedImage> extends LinkedList<T> {

    static class TimeRange {
        public TimeRange(long start, long end){
            this.start = start;
            this.end = end;
        }
        long start;
        long end;
    }

    public TimeRange mRange = new TimeRange(0,0);

    TimeRange getCacheFrameRange(){
        return mRange;
    }

    @Override
    public boolean add(T cacheSample)
    {
        boolean bRet;
        synchronized (this) {

            //find insert position
            int pos;
            for (pos = 0;pos < super.size(); pos++){
                if (get(pos).mTimeMs > cacheSample.mTimeMs)
                    break;
            }
            //first or insert position equal 0
            if (pos == 0)
                mRange.start = cacheSample.mTimeMs;

            if (pos == super.size()){
                mRange.end = cacheSample.mTimeMs;
                bRet = super.add(cacheSample);
            }else {
                super.add(pos, cacheSample);
                bRet = true;
            }
            this.notify();
        }
        return bRet;
    }

    @Override
    public T remove() {
//            if (isEmpty())
//                return null;
        T sample = null;

        synchronized (this) {

            while (super.size() <= 0){
                try {
                    this.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            sample = super.remove();

            if (!isEmpty()) {
                mRange.start = getFirst().mTimeMs;
            } else {
                mRange.start = mRange.end = 0;
            }
        }
        return sample;
    }

    @Override
    public void clear(){

        synchronized (this) {
            while (super.size() > 0){
                T sample = super.remove();
                if (sample != null){
                    sample.updateImage(false);
                }
            }
        }
    }
}