#include "Notification/LocalNotificationAndroid.h"

#if defined(__DAVAENGINE_ANDROID__)

#include "Notification/LocalNotificationController.h"
#include "Platform/TemplateAndroid/CorePlatformAndroid.h"
#include "Platform/TemplateAndroid/ExternC/AndroidLayer.h"
#include "Concurrency/LockGuard.h"
#include "Engine/Engine.h"

namespace DAVA
{
LocalNotificationAndroid::LocalNotificationAndroid(const String& _id)
#if defined(__DAVAENGINE_COREV2__)
    : notificationProvider("com/dava/engine/DavaNotificationProvider")
#else
    : notificationProvider("com/dava/framework/JNINotificationProvider")
#endif
{
    notificationId = _id;

    setText = notificationProvider.GetStaticMethod<void, jstring, jstring, jstring, jboolean>("NotifyText");
    setProgress = notificationProvider.GetStaticMethod<void, jstring, jstring, jstring, jint, jint, jboolean>("NotifyProgress");
    hideNotification = notificationProvider.GetStaticMethod<void, jstring>("HideNotification");
    enableTapAction = notificationProvider.GetStaticMethod<void, jstring>("EnableTapAction");

    notifyDelayed = notificationProvider.GetStaticMethod<void, jstring, jstring, jstring, jint, jboolean>("NotifyDelayed");
    removeAllDelayedNotifications = notificationProvider.GetStaticMethod<void>("RemoveAllDelayedNotifications");
}

void LocalNotificationAndroid::SetAction(const WideString& action)
{
    LockGuard<Mutex> mutexGuard(javaCallMutex);
    JNIEnv* env = JNI::GetEnv();
    jstring jstrNotificationUid = env->NewStringUTF(notificationId.c_str());
    enableTapAction(jstrNotificationUid);
    env->DeleteLocalRef(jstrNotificationUid);
}

void LocalNotificationAndroid::Hide()
{
    LockGuard<Mutex> mutexGuard(javaCallMutex);
    JNIEnv* env = JNI::GetEnv();
    jstring jstrNotificationUid = env->NewStringUTF(notificationId.c_str());
    hideNotification(jstrNotificationUid);
    env->DeleteLocalRef(jstrNotificationUid);
}

void LocalNotificationAndroid::ShowText(const WideString& title, const WideString& text, bool useSound)
{
    LockGuard<Mutex> mutexGuard(javaCallMutex);
    JNIEnv* env = JNI::GetEnv();

    jstring jstrNotificationUid = env->NewStringUTF(notificationId.c_str());

    jstring jStrTitle = JNI::ToJNIString(title);
    jstring jStrText = JNI::ToJNIString(text);

    setText(jstrNotificationUid, jStrTitle, jStrText, useSound);

    env->DeleteLocalRef(jstrNotificationUid);
    env->DeleteLocalRef(jStrTitle);
    env->DeleteLocalRef(jStrText);
}

void LocalNotificationAndroid::ShowProgress(const WideString& title, const WideString& text, const uint32 total, const uint32 progress, bool useSound)
{
    LockGuard<Mutex> mutexGuard(javaCallMutex);
    JNIEnv* env = JNI::GetEnv();

    jstring jstrNotificationUid = env->NewStringUTF(notificationId.c_str());

    jstring jStrTitle = JNI::ToJNIString(title);
    jstring jStrText = JNI::ToJNIString(text);

    setProgress(jstrNotificationUid, jStrTitle, jStrText, total, progress, useSound);

    env->DeleteLocalRef(jstrNotificationUid);
    env->DeleteLocalRef(jStrTitle);
    env->DeleteLocalRef(jStrText);
}

void LocalNotificationAndroid::PostDelayedNotification(const WideString& title, const WideString& text, int delaySeconds, bool useSound)
{
    LockGuard<Mutex> mutexGuard(javaCallMutex);
    JNIEnv* env = JNI::GetEnv();

    jstring jstrNotificationUid = env->NewStringUTF(notificationId.c_str());
    jstring jStrTitle = JNI::ToJNIString(title);
    jstring jStrText = JNI::ToJNIString(text);

    notifyDelayed(jstrNotificationUid, jStrTitle, jStrText, delaySeconds, useSound);

    env->DeleteLocalRef(jstrNotificationUid);
    env->DeleteLocalRef(jStrTitle);
    env->DeleteLocalRef(jStrText);
}

void LocalNotificationAndroid::RemoveAllDelayedNotifications()
{
    removeAllDelayedNotifications();
}

LocalNotificationImpl* LocalNotificationImpl::Create(const String& _id)
{
    return new LocalNotificationAndroid(_id);
}
}

extern "C"
{
#if defined(__DAVAENGINE_COREV2__)
JNIEXPORT void JNICALL Java_com_dava_engine_DavaNotificationProvider_onNotificationPressed(JNIEnv* env, jobject classthis, jstring uid)
#else
JNIEXPORT void JNICALL Java_com_dava_framework_JNINotificationProvider_onNotificationPressed(JNIEnv* env, jobject classthis, jstring uid)
#endif
{
#if defined(__DAVAENGINE_COREV2__)
    DAVA::String str(env->GetStringUTFChars(uid, 0));
    auto function = [str]()
    {
        DAVA::LocalNotificationController::Instance()->OnNotificationPressed(DAVA::String(str));
    };
    DAVA::Engine::Instance()->RunAsyncOnMainThread(function);
#else
    const char* str = env->GetStringUTFChars(uid, 0);
    DAVA::LocalNotificationController::Instance()->OnNotificationPressed(DAVA::String(str));
    env->ReleaseStringUTFChars(uid, str);
#endif
}
}

#endif
