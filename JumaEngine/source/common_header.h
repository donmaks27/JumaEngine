// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#if _DEBUG

#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#endif

#include <string>

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long int64;
typedef unsigned long uint64;

typedef char symbol;
typedef std::string text;

#define TEXT(str) u8 ## str
#define TO_TEXT(value) std::to_string(value)