
package com.xiaomi.crashpin;


public class CrashPin {

    public static native void testCrash();

    public static native void initial(String logFile);

//    public static void onCrashReport(String crashInfo) {
//        Log.d("CrashPin", crashInfo);
//    }

    static {
        System.loadLibrary("crashpin");
    }
}
