// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#define LOG_ENABLED _DEBUG

namespace std
{
    class mutex;
}

namespace JumaEngine
{
    class Log
    {
    private:
        Log() = default;

    public:

        static void error(const jstring& message) { error(jstring(), message); }
        static void error(const char* message) { error(jstring(), message); }
	    static void error(const jstring& method, const jstring& message) { WriteLog(PREFIX_ERROR, method, message); }
	    static void error(const jstring& method, const char* message) { WriteLog(PREFIX_ERROR, method, message); }

        static void warning(const jstring& message) { warning(jstring(), message); }
        static void warning(const char* message) { warning(jstring(), message); }
	    static void warning(const jstring& method, const jstring& message) { WriteLog(PREFIX_WARNING, method, message); }
	    static void warning(const jstring& method, const char* message) { WriteLog(PREFIX_WARNING, method, message); }
        
        static void info(const jstring& message) { info(jstring(), message); }
        static void info(const char* message) { info(jstring(), message); }
	    static void info(const jstring& method, const jstring& message) { WriteLog(PREFIX_INFO, method, message); }
	    static void info(const jstring& method, const char* message) { WriteLog(PREFIX_INFO, method, message); }
        
        static void correct(const jstring& message) { correct(jstring(), message); }
        static void correct(const char* message) { correct(jstring(), message); }
	    static void correct(const jstring& method, const jstring& message) { WriteLog(PREFIX_CORRECT, method, message); }
	    static void correct(const jstring& method, const char* message) { WriteLog(PREFIX_CORRECT, method, message); }

    private:

        static const char* PREFIX_ERROR;
	    static const char* PREFIX_WARNING;
	    static const char* PREFIX_INFO;
	    static const char* PREFIX_CORRECT;

#if LOG_ENABLED
        static std::mutex s_Mutex;
#endif

        
        static void WriteLog(const char* prefix, const jstring& method) { WriteLog(prefix, method, nullptr); }
        static void WriteLog(const char* prefix, const jstring& method, const jstring& message) { WriteLog(prefix, method, message.c_str()); }
        static void WriteLog(const char* prefix, const jstring& method, const char* message);
    };
}

#if LOG_ENABLED
#define JUMA_LOG(type, message) Log::type(jstring(__FUNCTION__) + JTEXT("(") + TO_JTEXT(__LINE__) + JTEXT(")"), message)
#define JUMA_LOG_EMPTY(type) JUMA_LOG(type, nullptr)
#else
#define JUMA_LOG(type, message)
#define JUMA_LOG_EMPTY(type)
#endif