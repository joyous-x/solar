//
// Created by jiao on 2016/8/20.
//

#ifndef STUDIO_JNITYPEUTILS_H
#define STUDIO_JNITYPEUTILS_H

#include <jni.h>


class jstring_2_const_char_ptr
{
    public:
        jstring_2_const_char_ptr(JNIEnv* env, jstring jstrObj);
        ~jstring_2_const_char_ptr();

    public:
        const char* get() const;

    private:
        JNIEnv*     m_Env;
        jstring     m_jstrObj;
        const char* m_szString;
};


class const_char_ptr_2_jstring
{
    public:
        const_char_ptr_2_jstring();
        const_char_ptr_2_jstring(JNIEnv* env, const char* szString);
        ~const_char_ptr_2_jstring();

    public:
        jstring get() const;

        void set(JNIEnv* env, const char* szString);
        void set(JNIEnv* env, jstring o);

        jstring detach();

    private:
        jstring _create_jstring(JNIEnv* env, const char* szString);

    private:
        JNIEnv*     m_Env;
        jstring     m_jstrObj;
};

#endif //STUDIO_JNITYPEUTILS_H
