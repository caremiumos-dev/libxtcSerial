#include <jni.h>
#include <android/log.h>

#include <cstddef>
#include <fcntl.h>
#include <unistd.h>

namespace {

const char kLogTag[] = "[F][serail]";

const int kOpenFlags = O_RDWR | O_NOCTTY | O_DSYNC | O_LARGEFILE;

}  // namespace

jint native_open(JNIEnv* env, jobject thiz, jstring path, jint baud) __attribute__((visibility("default")));
jint native_write(JNIEnv* env, jobject thiz, jint fd, jbyteArray data) __attribute__((visibility("default")));
jint native_read(JNIEnv* env, jobject thiz, jint fd, jobject buf, jint size) __attribute__((visibility("default")));
jint native_close(JNIEnv* env, jobject thiz, jint fd) __attribute__((visibility("default")));

jint native_open(JNIEnv* env, jobject thiz, jstring path, jint baud)
{
    (void)thiz;
    (void)baud;

    const char* cpath = env->GetStringUTFChars(path, nullptr);
    if (cpath == nullptr) {
        return -1;
    }

    int fd = open(cpath, kOpenFlags);
    if (fd != -1) {
        __android_log_print(ANDROID_LOG_DEBUG, kLogTag, "native_open fd = %d", fd);
    } else {
        __android_log_print(ANDROID_LOG_ERROR, kLogTag, "Cannot open port:%s", cpath);
    }

    env->ReleaseStringUTFChars(path, cpath);
    return fd;
}

jint native_write(JNIEnv* env, jobject thiz, jint fd, jbyteArray data)
{
    (void)thiz;

    if (fd <= 0) {
        __android_log_print(ANDROID_LOG_ERROR, kLogTag, "native_write fd error");
        return -1;
    }

    jsize len = env->GetArrayLength(data);
    jbyte* bytes = env->GetByteArrayElements(data, nullptr);
    if (bytes == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, kLogTag, "native_write data error");
        return -2;
    }

    jint ret = static_cast<jint>(write(fd, bytes, static_cast<size_t>(len)));
    env->ReleaseByteArrayElements(data, bytes, 0);
    return ret;
}

jint native_read(JNIEnv* env, jobject thiz, jint fd, jobject buf, jint size)
{
    (void)thiz;

    void* addr = env->GetDirectBufferAddress(buf);
    if (addr == nullptr) {
        return -1;
    }

    return static_cast<jint>(read(fd, addr, static_cast<size_t>(size)));
}

jint native_close(JNIEnv* env, jobject thiz, jint fd)
{
    (void)env;
    (void)thiz;
    return close(fd);
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) __attribute__((visibility("default")));

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;

    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, kLogTag, "jni_replace JVM ERROR:GetEnv");
        return -1;
    }

    jclass clazz = env->FindClass("com/android/internal/app/OpenFeatureSecureCheck");
    if (clazz == nullptr) {
        return -1;
    }

    static const JNINativeMethod kMethods[] = {
        {"open_port",  "(Ljava/lang/String;I)I",          reinterpret_cast<void*>(&native_open)},
        {"write_port", "(I[B)I",                          reinterpret_cast<void*>(&native_write)},
        {"read_port",  "(ILjava/nio/ByteBuffer;I)I",      reinterpret_cast<void*>(&native_read)},
        {"close_port", "(I)V",                            reinterpret_cast<void*>(&native_close)},
    };

    return env->RegisterNatives(clazz, kMethods, 4);
}