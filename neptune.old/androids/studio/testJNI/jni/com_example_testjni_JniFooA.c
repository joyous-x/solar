#include <string.h>
#include <jni.h>
#include <android/log.h>
#include "com_example_testjni_JniFooA.h"

JNIEXPORT jstring JNICALL Java_com_example_testjni_JniFooA_sayHello(JNIEnv *env, jclass class, jstring str)
{
	const char* name = (*env)->GetStringUTFChars(env, str, 0);
	char* hello = "hello everyone";
	
	char* result = malloc((strlen(name) + strlen(hello) + 1) * sizeof(char));
	memset(result, 0, sizeof(result));
	
	strcat(result, hello);
	strcat(result, name);
	
	(*env)->ReleaseStringUTFChars(env, str, name);
	
	str = (*env)->NewStringUTF(env, "hello JNI ~");
	free(result);

	__android_log_print(ANDROID_LOG_INFO, "JniFooA", "call from sayHello! \n");
	return str;
}