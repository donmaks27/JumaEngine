// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "log.h"
#include <mutex>
#include <iostream>

namespace JumaEngine
{
    const char* Log::PREFIX_ERROR   = JTEXT("[ERR]  ");
    const char* Log::PREFIX_WARNING = JTEXT("[WARN] ");
    const char* Log::PREFIX_INFO    = JTEXT("[INFO] ");
    const char* Log::PREFIX_CORRECT = JTEXT("[OK]   ");

#if LOG_ENABLED
    std::mutex Log::s_Mutex;
#endif

    void Log::WriteLog(const char* prefix, const jstring& method, const char* message)
    {
#if LOG_ENABLED
        std::unique_lock<std::mutex> lock(s_Mutex);

	    std::cout << prefix;
	    if (!method.empty())
	    {
	        std::cout << method << JTEXT(": ");
	    }
	    if ((message != nullptr) && (message[0] != '\0'))
	    {
	        std::cout << message;
	    }
        std::cout << std::endl;
#endif
    }
}

