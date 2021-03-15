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

        static void error(const text& message) { error(text(), message); }
        static void error(const char* message) { error(text(), message); }
	    static void error(const text& method, const text& message) { WriteLog(PREFIX_ERROR, method, message); }
	    static void error(const text& method, const char* message) { WriteLog(PREFIX_ERROR, method, message); }

        static void warning(const text& message) { warning(text(), message); }
        static void warning(const char* message) { warning(text(), message); }
	    static void warning(const text& method, const text& message) { WriteLog(PREFIX_WARNING, method, message); }
	    static void warning(const text& method, const char* message) { WriteLog(PREFIX_WARNING, method, message); }
        
        static void info(const text& message) { info(text(), message); }
        static void info(const char* message) { info(text(), message); }
	    static void info(const text& method, const text& message) { WriteLog(PREFIX_INFO, method, message); }
	    static void info(const text& method, const char* message) { WriteLog(PREFIX_INFO, method, message); }
        
        static void correct(const text& message) { correct(text(), message); }
        static void correct(const char* message) { correct(text(), message); }
	    static void correct(const text& method, const text& message) { WriteLog(PREFIX_CORRECT, method, message); }
	    static void correct(const text& method, const char* message) { WriteLog(PREFIX_CORRECT, method, message); }

    private:

        static const symbol* PREFIX_ERROR;
	    static const symbol* PREFIX_WARNING;
	    static const symbol* PREFIX_INFO;
	    static const symbol* PREFIX_CORRECT;

#if LOG_ENABLED
        static std::mutex s_Mutex;
#endif

        
        static void WriteLog(const symbol* prefix, const text& method) { WriteLog(prefix, method, nullptr); }
        static void WriteLog(const symbol* prefix, const text& method, const text& message);
        static void WriteLog(const symbol* prefix, const text& method, const char* message);
    };
}

#if LOG_ENABLED
#define JUMA_LOG(type, message) Log::type(text(TEXT(__FUNCTION__)) + TEXT("(") + TO_TEXT(__LINE__) + TEXT(")"), message)
#define JUMA_LOG_EMPTY(type) JUMA_LOG(type, nullptr)
#else
#define JUMA_LOG(type, message)
#define JUMA_LOG_EMPTY(type)
#endif