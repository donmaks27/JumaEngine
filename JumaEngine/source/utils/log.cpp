// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "log.h"
#include <mutex>
#include <iostream>

namespace JumaEngine
{
    const jchar* Log::PREFIX_ERROR   = TEXT("[ERR]  ");
    const jchar* Log::PREFIX_WARNING = TEXT("[WARN] ");
    const jchar* Log::PREFIX_INFO    = TEXT("[INFO] ");
    const jchar* Log::PREFIX_CORRECT = TEXT("[OK]   ");

#if LOG_ENABLED
    std::mutex Log::s_Mutex;
#endif

    void Log::WriteLog(const jchar* prefix, const jstring& method, const char* message)
    {
#if LOG_ENABLED
        std::unique_lock<std::mutex> lock(s_Mutex);

	    std::cout << prefix;
	    if (!method.empty())
	    {
	        std::cout << method << TEXT(": ");
	    }
	    if ((message != nullptr) && (message[0] != '\0'))
	    {
	        std::cout << message;
	    }
        std::cout << std::endl;
#endif
    }
}

