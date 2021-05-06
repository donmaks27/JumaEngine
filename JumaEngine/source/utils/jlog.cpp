// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "jlog.h"
#include <iostream>

namespace JumaEngine
{
    const char* jlog::PREFIX_ERROR   = JTEXT("[ERR]  ");
    const char* jlog::PREFIX_WARNING = JTEXT("[WARN] ");
    const char* jlog::PREFIX_INFO    = JTEXT("[INFO] ");
    const char* jlog::PREFIX_CORRECT = JTEXT("[OK]   ");

#if JLOG_ENABLED
    std::mutex jlog::s_Mutex;
#endif

    void jlog::WriteLog(const char* prefix, const jstring& method, const char* message)
    {
#if JLOG_ENABLED
        s_Mutex.lock();

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
        
        s_Mutex.unlock();
#endif
    }
}

