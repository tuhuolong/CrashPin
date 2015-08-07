//+----------------------------------------------------------------------------+
//|                                                                            |
//| Copyright(c) 2011, Dmitry "PVOID" Petuhov                                  |
//| All rights reserved.                                                       |
//|                                                                            |
//| Redistribution and use in source and binary forms, with or without         |
//| modification, are permitted provided that the following conditions are     |
//| met:                                                                       |
//|                                                                            |
//|   - Redistributions of source code must retain the above copyright notice, |
//|     this list of conditions and the following disclaimer.                  |
//|   - Redistributions in binary form must reproduce the above copyright      |
//|     notice, this list of conditions and the following disclaimer in the    |
//|     documentation and / or other materials provided with the distribution. |
//|                                                                            |
//| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        |
//| "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED  |
//| TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR |
//| PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR          |
//| CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,      |
//| EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,         |
//| PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         |
//| PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     |
//| LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING        |
//| NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         |
//| SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               |
//|                                                                            |
//+----------------------------------------------------------------------------+
#include "crashpin.h"
#include <jni.h>
#include <time.h>
#include <stdlib.h>
#include "log.h"

typedef void (TestFunc)();

class A {
public:
	//+----------------------------------------------------------------------------+
	//|                                                                            |
	//+----------------------------------------------------------------------------+
	static void crash() {
		//*(int *)0 = 10;
		srand(time(NULL));
		int a = rand();
		(*(TestFunc*) a)(); // Бабах!!!!
	}
	//+----------------------------------------------------------------------------+
	//|                                                                            |
	//+----------------------------------------------------------------------------+
	static void sayC() {
		crash();
	}
	//+----------------------------------------------------------------------------+
	//|                                                                            |
	//+----------------------------------------------------------------------------+
	static void sayB() {
		sayC();
	}
	//+----------------------------------------------------------------------------+
	//|                                                                            |
	//+----------------------------------------------------------------------------+
	static void sayA() {
		sayB();
	}
};

//+----------------------------------------------------------------------------+
//|                                                                            |
//+----------------------------------------------------------------------------+

JavaVM *jvm = NULL;
char* crashLogFile;
extern "C" jint JNI_OnLoad(JavaVM *vm, void* /*reserved*/) {
	jvm = vm;
	SCrashPin::Initialize();
	return JNI_VERSION_1_6;
}
//+----------------------------------------------------------------------------+
//|                                                                            |
//+----------------------------------------------------------------------------+
extern "C" void Java_com_xiaomi_crashpin_CrashPin_testCrash(JNIEnv *env,
		jobject self) {
	A::sayA();
}
//+----------------------------------------------------------------------------+
extern "C" void Java_com_xiaomi_crashpin_CrashPin_initial(JNIEnv *env,
		jobject self, jstring filePath) {
	crashLogFile = (char *) env->GetStringUTFChars(filePath, 0);
}

void reportCrash(char* info) {
//	JNIEnv *env = 0;
//	int result = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
//	if (result != JNI_OK) {
//		LOGE("%s", "jvm->GetEnv null");
//		return;
//	}
//	jclass crashPinClass = env->FindClass("com/xiaomi/crashpin/CrashPin");
//	if (crashPinClass == NULL) {
//		LOGE("%s", "FindClass CrashPin null");
//		return;
//	}
//	jmethodID crashReportMethod = env->GetStaticMethodID(crashPinClass,
//			"onCrashReport", "(Ljava/lang/String;)V");
//	if (crashReportMethod == NULL) {
//		LOGE("%s", "GetMethod onCrashReport null");
//		return;
//	}
//	jstring crashInfo = env->NewStringUTF(info);
//	env->CallStaticVoidMethod(crashPinClass, crashReportMethod, crashInfo);
}

