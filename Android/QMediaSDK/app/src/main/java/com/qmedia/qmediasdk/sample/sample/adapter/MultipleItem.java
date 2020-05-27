package com.qmedia.qmediasdk.sample.sample.adapter;


import com.qmedia.qmediasdk.sample.sample.bean.FileInfo;

/**
 * Created by CWJ on 2017/3/20.
 */

public class MultipleItem {
    public static final int FOLD = 1;
    public static final int FILE = 2;
    private int itemType;
    private FileInfo data ;

    public MultipleItem(int itemType, FileInfo data) {
        this.data = data;
        this.itemType = itemType;
    }

    public FileInfo getData() {
        return data;
    }


    public int getItemType() {
        return itemType;
    }
}
