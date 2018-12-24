package com.david.verusalive;

public class Watcher {

    static {
        System.loadLibrary("native-lib");
    }

    public native void createWatcher(String userId);

    public native void createConnect();
}
