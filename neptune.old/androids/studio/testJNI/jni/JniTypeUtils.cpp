#include "JniTypeUtils.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

jstring_2_const_char_ptr::jstring_2_const_char_ptr(JNIEnv* env, jstring jstrObj)
    : m_Env(env), m_jstrObj(jstrObj), m_szString(NULL)
{
    if (NULL == m_Env || NULL == m_jstrObj)
    {
        return;
    }

    m_szString = m_Env->GetStringUTFChars(m_jstrObj, NULL);
    if (env->ExceptionCheck())
    {
        env->ExceptionClear();
        m_szString = NULL;
    }
}

jstring_2_const_char_ptr::~jstring_2_const_char_ptr()
{
    if (NULL == m_Env || NULL == m_jstrObj || NULL == m_szString)
    {
        return;
    }

    m_Env->ReleaseStringUTFChars(m_jstrObj, m_szString);
}

const char* jstring_2_const_char_ptr::get() const
{
    return m_szString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const_char_ptr_2_jstring::const_char_ptr_2_jstring()
    : m_Env(NULL), m_jstrObj(NULL)
{
}

const_char_ptr_2_jstring::const_char_ptr_2_jstring(JNIEnv* env, const char* szString)
    : m_Env(env), m_jstrObj(NULL)
{
    if (NULL == m_Env || NULL == szString)
    {
        return;
    }

    m_jstrObj = _create_jstring(env, szString);
}

const_char_ptr_2_jstring::~const_char_ptr_2_jstring()
{
    if (NULL == m_Env || NULL == m_jstrObj)
    {
        return;
    }

    m_Env->DeleteLocalRef(m_jstrObj);
}

jstring const_char_ptr_2_jstring::get() const
{
    return m_jstrObj;
}

void const_char_ptr_2_jstring::set(JNIEnv* env, const char* szString)
{
    if (NULL != m_Env && NULL != m_jstrObj)
    {
        m_Env->DeleteLocalRef(m_jstrObj);
    }

    m_Env = NULL;
    m_jstrObj = NULL;

    if (NULL == env || NULL == szString)
    {
        return;
    }

    m_Env = env;
    m_jstrObj = _create_jstring(env, szString);
}

void const_char_ptr_2_jstring::set(JNIEnv* env, jstring o)
{
    if (m_Env == env && m_jstrObj == o)
    {
        return;
    }

    if (NULL != m_Env && NULL != m_jstrObj)
    {
        m_Env->DeleteLocalRef(m_jstrObj);
    }

    m_Env = env;
    m_jstrObj = o;
}

jstring const_char_ptr_2_jstring::detach()
{
    jstring rst = m_jstrObj;
    m_Env = NULL;
    m_jstrObj = NULL;
    return rst;
}

jstring const_char_ptr_2_jstring::_create_jstring(JNIEnv* env, const char* szString)
{
    assert(NULL != env);
    assert(NULL != szString);

    jstring str = env->NewStringUTF(szString);
    return str;
}