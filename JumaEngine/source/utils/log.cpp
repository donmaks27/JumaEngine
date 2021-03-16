// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "log.h"
#include <mutex>
#include <iostream>

namespace JumaEngine
{
    const symbol* Log::PREFIX_ERROR   = TEXT("[ERR]  ");
    const symbol* Log::PREFIX_WARNING = TEXT("[WARN] ");
    const symbol* Log::PREFIX_INFO    = TEXT("[INFO] ");
    const symbol* Log::PREFIX_CORRECT = TEXT("[OK]   ");

#if LOG_ENABLED
    std::mutex Log::s_Mutex;
#endif

    void Log::WriteLog(const symbol* prefix, const text& method, const char* message)
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

