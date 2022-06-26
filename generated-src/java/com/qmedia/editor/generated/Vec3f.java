// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

package com.qmedia.editor.generated;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class Vec3f {


    /*package*/ final float mV1;

    /*package*/ final float mV2;

    /*package*/ final float mV3;

    public Vec3f(
            float v1,
            float v2,
            float v3) {
        this.mV1 = v1;
        this.mV2 = v2;
        this.mV3 = v3;
    }

    // consturct from json
    public Vec3f(JSONObject json) throws JSONException {
        this.mV1 = json.getDouble("v1");
        this.mV2 = json.getDouble("v2");
        this.mV3 = json.getDouble("v3");
    }

    public float getV1() {
        return mV1;
    }

    public float getV2() {
        return mV2;
    }

    public float getV3() {
        return mV3;
    }

    @Override
    public boolean equals(@Nullable Object obj) {
        if (!(obj instanceof Vec3f)) {
            return false;
        }
        Vec3f other = (Vec3f) obj;
        return this.mV1 == other.mV1 &&
                this.mV2 == other.mV2 &&
                this.mV3 == other.mV3;
    }

    @Override
    public int hashCode() {
        // Pick an arbitrary non-zero starting value
        int hashCode = 17;
        hashCode = hashCode * 31 + Float.floatToIntBits(mV1);
        hashCode = hashCode * 31 + Float.floatToIntBits(mV2);
        hashCode = hashCode * 31 + Float.floatToIntBits(mV3);
        return hashCode;
    }

    public JSONObject toJson() throws JSONException {
        JSONObject root = new JSONObject();
        root.put("v1", this.mV1);
        root.put("v2", this.mV2);
        root.put("v3", this.mV3);
        return root;
    }

    @Override
    public String toString() {
        return "Vec3f{" +
                "mV1=" + mV1 +
                "," + "mV2=" + mV2 +
                "," + "mV3=" + mV3 +
        "}";
    }

}
