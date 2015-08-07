#ifndef MICRORAPID_LOG_H
#define MICRORAPID_LOG_H

#include<android/log.h>
#define LOG_TAG "CrashPin"

//#define LOGI(fmt, args...)
//#define LOGD(fmt, args...)
//#define LOGE(fmt, args...)
//#define LOG_ENTER_FUNC()
//#define LOG_QUITE_FUNC()

#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args)
//
//#define LOG_ENTER_FUNC()   LOGD("enter %s\n", __FUNCTION__)
//#define LOG_QUITE_FUNC()   LOGD("quite %s\n", __FUNCTION__)

#endif

