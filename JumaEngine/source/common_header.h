// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#if _DEBUG

#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#endif

#include <string>

#define JEPSILON 0.0001f

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long int64;
typedef unsigned long uint64;

typedef char jchar;
typedef std::string jstring;

#define JTEXT(str) u8 ## str
#define TO_JTEXT(value) std::to_string(value)