LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := crashpin
LOCAL_SRC_FILES := jni_crashpin.cpp crashpin.cpp map.cpp stacktrace.cpp
LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)