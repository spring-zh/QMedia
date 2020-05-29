package com.qmedia.qmediasdk.QCommon.media;

import java.util.LinkedList;

//this queue use to cache packets
public class EncodedPacketQueue extends LinkedList<HardwareDecoder.EncodedPacket> {
    @Override
    public boolean add(HardwareDecoder.EncodedPacket packet)
    {
        synchronized (this) {
            boolean bRet = super.add(packet);
            this.notify();
            return bRet;
        }
    }

    @Override
    public HardwareDecoder.EncodedPacket remove() {
        HardwareDecoder.EncodedPacket packet = null;
        synchronized (this) {

            while (super.size() <= 0){
                try {
                    this.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            packet = super.remove();
        }
        return packet;
    }

    @Override
    public void clear(){
        synchronized (this) {
            super.clear();
        }
    }
}