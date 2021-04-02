// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#define GLM_FORCE_INLINE
//#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_XYZW_ONLY

#if _DEBUG

#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#endif

#include "utils/jstring.h"

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long int64;
typedef unsigned long uint64;

#define JTEXT(str) u8 ## str
#define TO_JTEXT(value) std::to_string(value)