package com.qmedia.qmediasdk.QCommon;

import java.util.LinkedList;

//use to cache decode output buffer sort by pts
public class FrameCacheQueue<T extends HardwareDecoder.DecodedFrame> extends LinkedList<T> {

    private boolean isAbort = false;

    private Object signal = new Object();
    private QRange cacheTimeRange = new QRange(0,0);

    //get the cache frame timestamp range
    public QRange getCacheFrameRange(){
        return cacheTimeRange;
    }

    public boolean isAbort() {
        return isAbort;
    }

    public void setAbort(boolean abort) {
        synchronized (signal) {
            isAbort = abort;
            signal.notify();
        }
    }

    @Override
    public boolean add(T cacheSample)
    {
        boolean bRet = false;
        synchronized (signal) {

            if (cacheSample != null) {
                //find insert position
                int pos;
                for (pos = 0;pos < super.size(); pos++){
                    if (get(pos).mTimeMs > cacheSample.mTimeMs)
                        break;
                }
                //first or insert position equal 0
                if (pos == 0)
                    cacheTimeRange.start = cacheSample.mTimeMs;

                if (pos == super.size()){
                    cacheTimeRange.end = cacheSample.mTimeMs;
                    bRet = super.add(cacheSample);
                }else {
                    super.add(pos, cacheSample);
                    bRet = true;
                }
            }
            //send notify to remove()' wait
            signal.notify();
        }
        return bRet;
    }

    //will block until add frame or isAbort set
    @Override
    public T remove() {

        T sample = null;

        synchronized (signal) {

            while (super.isEmpty() && !isAbort){
                try {
                    signal.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            if (!isEmpty()) {
                sample = super.remove();

                if (!isEmpty()) {
                    cacheTimeRange.start = getFirst().mTimeMs;
                }
            }
        }
        return sample;
    }

    @Override
    public void clear(){

        synchronized (signal) {
//            while (super.size() > 0){
//                T sample = super.remove();
//                if (sample != null){
//                    sample.updateImage(false);
//                }
//            }
            super.clear();
        }
        cacheTimeRange.start = cacheTimeRange.end = 0;
    }
}