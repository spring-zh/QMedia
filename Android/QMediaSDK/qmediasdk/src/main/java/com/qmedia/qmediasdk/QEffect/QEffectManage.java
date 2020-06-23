package com.qmedia.qmediasdk.QEffect;

public class QEffectManage {
    /**
     * get all effect config, {name , describe , type}
     */
    public static QEffectConfig[] getAllEffectConfig() {
        return native_getAllEffectConfig();
    }

    static boolean  addFilterConfig(String json_config) {
        return native_addFilterConfig(json_config);
    }

    /**
     * create effect by effect_name
     * @param effect_name: use name in the name of all EffectConfig
     * @return failed is null
     */
    public static QEffect createEffect(String effect_name) {
        long effect_ptr = native_createEffect(effect_name);
        if (effect_ptr != 0) {
            return new QEffect(effect_ptr);
        }else
            return null;
    }

    private static native QEffectConfig[] native_getAllEffectConfig();
    private static native boolean native_addFilterConfig(String json_config);
    private static native long native_createEffect(String effect_name);
}
