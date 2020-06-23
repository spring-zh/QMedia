package com.qmedia.qmediasdk.QEffect;

public class QEffectConfig {
    public QEffectConfig(String name, String describe, String type) {
        this.name = name;
        this.describe = describe;
        this.type = type;
    }

    public String getName() {
        return name;
    }

    public String getDescribe() {
        return describe;
    }

    public String getType() {
        return type;
    }

    String name;
    String describe;
    String type;
}
