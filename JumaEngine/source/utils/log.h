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

        static void error(const std::string& message) { error(std::string(), message); }
        static void error(const char* message) { error(std::string(), message); }
	    static void error(const std::string& method, const std::string& message) { WriteLog(PREFIX_ERROR, method, message); }
	    static void error(const std::string& method, const char* message) { WriteLog(PREFIX_ERROR, method, message); }

        static void warning(const std::string& message) { warning(std::string(), message); }
        static void warning(const char* message) { warning(std::string(), message); }
	    static void warning(const std::string& method, const std::string& message) { WriteLog(PREFIX_WARNING, method, message); }
	    static void warning(const std::string& method, const char* message) { WriteLog(PREFIX_WARNING, method, message); }
        
        static void info(const std::string& message) { info(std::string(), message); }
        static void info(const char* message) { info(std::string(), message); }
	    static void info(const std::string& method, const std::string& message) { WriteLog(PREFIX_INFO, method, message); }
	    static void info(const std::string& method, const char* message) { WriteLog(PREFIX_INFO, method, message); }
        
        static void correct(const std::string& message) { correct(std::string(), message); }
        static void correct(const char* message) { correct(std::string(), message); }
	    static void correct(const std::string& method, const std::string& message) { WriteLog(PREFIX_CORRECT, method, message); }
	    static void correct(const std::string& method, const char* message) { WriteLog(PREFIX_CORRECT, method, message); }

    private:

        static const char* PREFIX_ERROR;
	    static const char* PREFIX_WARNING;
	    static const char* PREFIX_INFO;
	    static const char* PREFIX_CORRECT;

#if LOG_ENABLED
        static std::mutex s_Mutex;
#endif

        
        static void WriteLog(const char* prefix, const std::string& method) { WriteLog(prefix, method, nullptr); }
        static void WriteLog(const char* prefix, const std::string& method, const std::string& message) { WriteLog(prefix, method, message.c_str()); }
        static void WriteLog(const char* prefix, const std::string& method, const char* message);
    };
}

#if LOG_ENABLED
#define JUMA_LOG(type, message) Log::type(std::string(__FUNCTION__) + JTEXT("(") + TO_JTEXT(__LINE__) + JTEXT(")"), message)
#define JUMA_LOG_EMPTY(type) JUMA_LOG(type, nullptr)
#else
#define JUMA_LOG(type, message)
#define JUMA_LOG_EMPTY(type)
#endif